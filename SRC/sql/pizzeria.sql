drop database if exists pizzeriadb;
create database pizzeriadb;
use pizzeriadb;

-- passwd SHA1
drop table if exists UtenteLavoratore;
create table UtenteLavoratore (
	Username varchar(10) primary key,
	Nome varchar(20) not null,
	Cognome varchar(20) not null,
	ComuneResidenza varchar(34) not null,
	DataNascita date not null,
	ComuneNascita varchar(34) not null,
	CF char(16) not null,
	Passwd char(40) not null,
	Ruolo tinyint not null,
	unique (
		CF
	),
	check (
		Ruolo in (1,2,3,4)
	)
);

drop table if exists Tavolo;
create table Tavolo (
	NumTavolo smallint primary key,
	MaxCommensali tinyint not null,
	IsOccupato boolean not null default false
);

drop table if exists Ingrediente;
create table Ingrediente (
	Nome varchar(20) primary key,
	NumDisponibilitaScorte int not null,
	CostoAlKg float not null,
	check(
		CostoAlKg > 0
	)
);

drop table if exists ProdottoNelMenu;
create table ProdottoNelMenu (
	Nome varchar(20) primary key,
	CostoUnitario float not null,
	IsBarMenu boolean not null,
	IsAlcolico boolean,
	check(
		CostoUnitario > 0 and
		((IsBarMenu and IsAlcolico is not NULL) or 
			(not IsBarMenu and isAlcolico is NULL))
	)
);

drop table if exists ComposizioneProdotto;
create table ComposizioneProdotto(
	NomeProdotto varchar(20) 
		references ProdottoNelMenu(Nome) 
		on delete cascade,
	NomeIngrediente varchar(20) 
		references Ingrediente(Nome)
		on delete no action,
	primary key(
		NomeProdotto, 
		NomeIngrediente
	)
);

drop table if exists Turno;
create table Turno (
	DataOraInizio timestamp primary key,
	DataOraFine timestamp not null,
	check(
		DataOraFine > DataOraInizio
	)
);

drop table if exists TurnoAssegnato;
create table TurnoAssegnato (
	Turno timestamp 
		references Turno(DataOraInizio)
		on delete no action,
	Tavolo smallint
		references Tavolo(NumTavolo)
		on delete no action,
	Cameriere varchar(10)
		references UtenteLavoratore(Username)
		on delete no action,
	primary key(
		Turno, 
		Tavolo, 
		Cameriere
	)
);

drop table if exists TavoloOccupato;
create table TavoloOccupato (
	DataOraOccupazione timestamp primary key,
	Nome varchar(20) not null,
	Cognome varchar(20) not null,
	NumCommensali tinyint unsigned not null,
	IsServitoAlmenoUnaVolta boolean not null default false,
	Tavolo smallint not null references Tavolo(NumTavolo)
);

drop table if exists Scontrino;
create table Scontrino (
	IdFiscale int auto_increment primary key,
	DataOraEmissione timestamp not null,
	CostoTotale float not null,
	IsPagato boolean not null default false,
	TavoloOccupato timestamp not null
		 references TavoloOccupato(DataOraOccupazione),
	check(
		CostoTotale > 0
	),
	unique(
		DataOraEmissione
	)
);

delimiter !

drop trigger if exists TurnoCheckDataOraInizio_Insert!
create trigger TurnoCheckDataOraInizio_Insert
before insert on Turno for each row
begin	
	if NEW.DataOraInizio < now() then
		signal sqlstate '45001' 
			set message_text = "Impossibile creare turno per il passato";
	end if;
end!

drop trigger if exists TurnoCheckOverlap_Insert!
create trigger TurnoCheckOverlap_Insert
before insert on Turno for each row
begin
	declare overlapCount int;

	select
		count(*)
	from
		Turno
	where
		DataOraInizio <= NEW.DataOraFine and DataOraFine >= NEW.DataOraInizio
	into
		overlapCount;

	if overlapCount > 0 then
		signal sqlstate '45003'
			set message_text = "Overlap dei turni";
	end if;
end!

drop trigger if exists TurnoAssegnatoCheckIsCameriere_Insert!
create trigger TurnoAssegnatoCheckIsCameriere_Insert
before insert on TurnoAssegnato for each row
begin
	declare newUserRole tinyint;

	select
		Ruolo
	from
		UtenteLavoratore
	where
		Username = NEW.Cameriere
	into
		newUserRole;
	
	if newUserRole <> 2 then
		signal sqlstate '45002'
			set message_text = "Impossibile assegnare un turno/tavolo a un non-cameriere";
	end if;
end!

drop trigger if exists ScontrinoCheckServizio_Insert!
create trigger ScontrinoCheckServizio_Insert
before insert on Scontrino for each row
begin
	declare isServito boolean;

	select
		IsServitoAlmenoUnaVolta
	from
		TavoloOccupato T
	where
		T.DataOraOccupazione = NEW.TavoloOccupato
	into
		isServito;

	if isServito = false then
		signal sqlstate '45004'
			set message_text = "il tavolo deve essere servito almeno una volta";
	end if;
end!

drop trigger if exists ScontrinoCheckPagato_AfterUpdate!
create trigger ScontrinoCheckPagato_AfterUpdate
after update on Scontrino for each row
begin
	if NEW.IsPagato = true then
		update
			Tavolo
		set
			IsOccupato = false
		where
			NumTavolo = (
				select 
					NumTavolo
				from
					Scontrino S join TavoloOccupato O
				where
					O.DataOraOccupazione = NEW.TavoloOccupato);
	end if;
end!

drop procedure if exists TentaLogin!
create procedure TentaLogin(in usern varchar(10), in pwd varchar(45), out userRole tinyint)
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	set userRole = 0;
	
	start transaction;

	select
		Ruolo
	from
		UtenteLavoratore
	where
		Username = usern and Passwd = SHA1(pwd)
	into
		userRole;

	commit;
end!

drop procedure if exists RegistraUtente!
create procedure RegistraUtente(
	in username varchar(10), 
	in nome varchar(20),
	in cognome varchar(20),
	in comuneResidenza varchar(34),
	in dataNascita date,
	in comuneNascita varchar(34),
	in cf char(16),
	in passwd varchar(45),
	in ruolo tinyint)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into
		UtenteLavoratore(Username, 
							Nome, 
							Cognome, 
							ComuneResidenza, 
							DataNascita, 
							ComuneNascita, 
							CF, 
							Passwd, 
							Ruolo)
	values(
		username,
		nome,
		cognome,
		comuneResidenza,
		dataNascita,
		comuneNascita,
		cf,
		SHA1(passwd),
		ruolo
	);

	commit;
end!

drop procedure if exists RipristinoPassword!
create procedure RipristinoPassword(in usern varchar(10), in pwd varchar(45))
begin
	set transaction isolation level read uncommitted;

	start transaction;

	update
		UtenteLavoratore
	set
		Passwd = SHA1(pwd)
	where
		Username = usern;
	
	commit;
end!

drop procedure if exists AggiungiNuovoTavolo!
create procedure AggiungiNuovoTavolo(in numt smallint, in maxcomm tinyint)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into 
		Tavolo(NumTavolo, MaxCommensali)
	values
		(numt, maxcomm);

	commit;
end!

drop procedure if exists AggiungiNuovoIngrediente!
create procedure AggiungiNuovoIngrediente(
	in nomeIng varchar(20), 
	in dispIniz int, 
	in costoPerKg float)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into
		Ingrediente(Nome, NumDisponibilitaScorte, CostoAlKg)
	values
		(nomeIng, dispIniz, costoPerKg);

	commit;
end!

drop procedure if exists AggiungiProdottoNelMenu!
create procedure AggiungiProdottoNelMenu(
		in prodNome varchar(20), 
		in prodCostoUn float,
		in prodMenuBar boolean,
		in prodAlcolico boolean)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into
		ProdottoNelMenu(Nome, CostoUnitario, IsBarMenu, IsAlcolico)
	values
		(prodNome, prodCostoUn, prodMenuBar, prodAlcolico);

	commit;
end!

drop procedure if exists AssociaProdottoAIngrediente!
create procedure AssociaProdottoAIngrediente(in nomeProd varchar(20), in nomeIng varchar(20))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into 
		ComposizioneProdotto(NomeProdotto, NomeIngrediente)
	values
		(nomeProd, nomeIng);

	commit;
end!

drop procedure if exists AggiungiTurno!
create procedure AggiungiTurno(in inizio timestamp, in fine timestamp)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into 
		Turno(DataOraInizio, DataOraFine)
	values
		(inizio, fine);

	commit;
end!

drop procedure if exists RimuoviProdottoNelMenu!
create procedure RimuoviProdottoNelMenu(in nomeProd varchar(20))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	delete from
		ProdottoNelMenu
	where
		Nome = nomeProd;

	commit;
end!

drop procedure if exists RimuoviIngrediente!
create procedure RimuoviIngrediente(in nomeIng varchar(20))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;
	
	set transaction isolation level read uncommitted;

	start transaction;

	delete from
		Ingrediente
	where
		Nome = nomeIng;

	commit;
end!

drop procedure if exists RimuoviAssocProdottoEIngrediente!
create procedure RimuoviAssocProdottoEIngrediente(in nomeProd varchar(20), in nomeIng varchar(20))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;
	
	set transaction isolation level read uncommitted;

	start transaction;

	delete from
		ComposizioneProdotto
	where
		NomeProdotto = nomeProd and NomeIngrediente = nomeIng;

	commit;
end!

drop procedure if exists OttieniTurni!
create procedure OttieniTurni()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		DataOraInizio, DataOraFine
	from
		Turno
	where
		now() <= DataOraFine;

	commit;
end!

drop procedure if exists OttieniUtenti!
create procedure OttieniUtenti()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		Username, Nome, Cognome, CF, ComuneResidenza, 
		ComuneNascita, DataNascita, Ruolo
	from
		UtenteLavoratore;

	commit;
end!

drop procedure if exists OttieniTavoli!
create procedure OttieniTavoli()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	set @current_time = now();

	start transaction;

	select
		NumTavolo, MaxCommensali, IsOccupato, exists (
				select
					*
				from
					TurnoAssegnato Ta join Turno Tu on 
						Ta.Turno = Tu.DataOraInizio
				where 
					Ta.Tavolo = NumTavolo and 
						Tu.DataOraInizio <= @current_time and 
						@current_time <= Tu.DataOraFine)
	from
		Tavolo;

	commit;
end!

drop procedure if exists OttieniTurniAssegnati!
create procedure OttieniTurniAssegnati()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		Tu.DataOraInizio, Tu.DataOraFine, Ta.Tavolo, U.Nome, U.Cognome, U.Username
	from
		(TurnoAssegnato Ta join Turno Tu on
			Ta.Turno = Tu.DataOraInizio) join UtenteLavoratore U on
				 Ta.Cameriere = U.Username
	where
		now() <= Tu.DataOraFine
	order by
		Tu.DataOraInizio;

	commit;
end!

drop procedure if exists OttieniTurnoAttuale!
create procedure OttieniTurnoAttuale()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	set @current_time = now();

	start transaction;

	select
		Tu.DataOraInizio, Tu.DataOraFine, Ta.Tavolo, U.Nome, U.Cognome, U.Username
	from
		(TurnoAssegnato Ta right join Turno Tu on
			Ta.Turno = Tu.DataOraInizio) left join UtenteLavoratore U on
				 Ta.Cameriere = U.Username
	where
		Tu.DataOraInizio <= @current_time and
			@current_time <= Tu.DataOraFine;

	commit;
end!

drop procedure if exists AssegnaTurno!
create procedure AssegnaTurno(
		in numTavolo smallint,
		in dataOraInizioTurno timestamp, 
		in cameriereUsername varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	insert into 
		TurnoAssegnato(Tavolo, Turno, Cameriere)
	values
		(numTavolo, dataOraInizioTurno, cameriereUsername);

	commit;
end!

drop procedure if exists OttieniMenu!
create procedure OttieniMenu()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		Nome, IsBarMenu, IsAlcolico, CostoUnitario
	from
		ProdottoNelMenu
	order by
		IsBarMenu, Nome asc;

	commit;
end!

drop procedure if exists OttieniComposizioneProdotto!
create procedure OttieniComposizioneProdotto()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		NomeProdotto, NomeIngrediente
	from
		ComposizioneProdotto;

	commit;
end!

drop procedure if exists OttieniIngredienti!
create procedure OttieniIngredienti()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		Nome, NumDisponibilitaScorte, CostoAlKg
	from
		Ingrediente;

	commit;
end!

drop procedure if exists IncDispIngrediente!
create procedure IncDispIngrediente(in nomeIng varchar(20), in incBy int)
begin
	set transaction isolation level read uncommitted;

	start transaction;

	update
		Ingrediente
	set
		NumDisponibilitaScorte = NumDisponibilitaScorte + incBy
	where
		Nome = nomeIng;

	commit;
end!

drop function if exists InTimeRange!
create function InTimeRange(monthly boolean, tm timestamp)
returns boolean deterministic
begin
	set @current_time = now();
	set @by_month = YEAR(@current_time) = YEAR(tm) and
			MONTH(@current_time) = MONTH(tm);

	if monthly then
		return @by_month;
	end if;

	return @by_month and DAY(@current_time) = DAY(tm);
end!

drop procedure if exists OttieniEntrate!
create procedure OttieniEntrate(in mensili boolean)
begin
	set transaction read only;
	set transaction isolation level repeatable read;

	start transaction;

	select
		count(*) as NumScontrini,
		sum(CostoTotale) as IncassoTotale
	from
		Scontrino
	where
		IsPagato and InTimeRange(mensili, DataOraEmissione);

	select
		IdFiscale, DataOraEmissione, CostoTotale
	from
		Scontrino
	where
		IsPagato and InTimeRange(mensili, DataOraEmissione);

	commit;
end!

drop procedure if exists OttieniScontriniNonPagati!
create procedure OttieniScontriniNonPagati()
begin
	set transaction read only;
	set transaction isolation level read uncommitted;

	start transaction;

	select
		IdFiscale, DataOraEmissione, CostoTotale
	from
		Scontrino
	where
		IsPagato = false;

	commit;
end!

drop procedure if exists ContrassegnaScontinoPagato!
create procedure ContrassegnaScontrinoPagato(in idFisc int)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read uncommitted;

	start transaction;

	update 
		Scontrino
	set
		IsPagato = true
	where
		IdFiscale = idFisc and IsPagato = false;

	commit;
end!

drop procedure if exists AssegnaTavoloACliente!
create procedure AssegnaTavoloACliente(
		in cliNome varchar(20), 
		in cliCognome varchar(20), 
		in numComm tinyint)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @current_time = now();

	set transaction isolation level repeatable read;
	start transaction;

	set @numTavoloAdatto = (
		select
			NumTavolo
		from
			(Tavolo T join TurnoAssegnato Ta on
				T.NumTavolo = Ta.Tavolo) join Turno Tu on 
					Ta.Turno = Tu.DataOraInizio
		where
			T.IsOccupato = false and 
			Tu.DataOraInizio <= @current_time and 
			@current_time <= Tu.DataOraFine and
			numComm <= T.MaxCommensali
		limit 1);

	if @numTavoloAdatto is NULL then
		signal sqlstate '45005' 
			set message_text = "Nessun tavolo disponibile per l'assegnazione";
	end if;

	insert into 
		TavoloOccupato(DataOraOccupazione, Nome, Cognome, NumCommensali, Tavolo)
	values
		(now(), cliNome, cliCognome, numComm, @numTavoloAdatto);

	update
		Tavolo
	set
		IsOccupato = true
	where
		Tavolo = @numTavoloAdatto;

	select @numTavoloAdatto as NumTavolo;

	commit;
end!

-- TODO -- TODO -- xxx xxx xxx 

drop procedure if exists OttieniTavoliScontrinoStampabile!
create procedure OttieniTavoliScontrinoStampabile()
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	commit;
end!

drop procedure if exists StampaScontrino!
create procedure StampaScontrino(in dataOraOcc timestamp)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	set @scontrino = (
				select
					* 
				from
					Scontrino
				where
					TavoloOccupato = dataOraOc);

	if @scontrino is not NULL then
		signal sqlstate '45006' 
			set message_text = "Scontrino già stampato";
	end if;

	insert into 
		Scontrino(DataOraEmisione, CostoTotale, TavoloOccupato)
	values
		(now(), 0, dataOraOcc);
	
	commit;
end!

-- TODO -- TODO -- xxx xxx xxx 

delimiter ;

drop user if exists login;
create user 'login'@'%' identified by 'login';

drop user if exists pizzaiolo;
create user 'pizzaiolo'@'%' identified by 'pizzaiolo';

drop user if exists barman;
create user 'barman'@'%' identified by 'barman';

drop user if exists cameriere;
create user 'cameriere'@'%' identified by 'cameriere';

drop user if exists manager;
create user 'manager'@'%' identified by 'manager';

grant execute on procedure TentaLogin to 'login';

grant execute on procedure RegistraUtente to 'manager';
grant execute on procedure RipristinoPassword to 'manager';
grant execute on procedure AggiungiNuovoTavolo to 'manager';
grant execute on procedure AggiungiNuovoIngrediente to 'manager';
grant execute on procedure AggiungiProdottoNelMenu to 'manager';
grant execute on procedure AssociaProdottoAIngrediente to 'manager';
grant execute on procedure AggiungiTurno to 'manager';
grant execute on procedure RimuoviProdottoNelMenu to 'manager';
grant execute on procedure RimuoviIngrediente to 'manager';
grant execute on procedure RimuoviAssocProdottoEIngrediente to 'manager';
grant execute on procedure OttieniTurni to 'manager';
grant execute on procedure OttieniUtenti to 'manager';
grant execute on procedure OttieniTavoli to 'manager';
grant execute on procedure AssegnaTurno to 'manager';
grant execute on procedure OttieniTurnoAttuale to 'manager';
grant execute on procedure OttieniTurniAssegnati to 'manager';
grant execute on procedure OttieniMenu to 'manager';
grant execute on procedure OttieniComposizioneProdotto to 'manager';
grant execute on procedure OttieniIngredienti to 'manager';
grant execute on procedure IncDispIngrediente to 'manager';
grant execute on procedure OttieniEntrate to 'manager';
grant execute on procedure OttieniScontriniNonPagati to 'manager';
grant execute on procedure ContrassegnaScontrinoPagato to 'manager';
grant execute on procedure AssegnaTavoloACliente to 'manager';
grant execute on procedure OttieniTavoliScontrinoStampabile to 'manager';
grant execute on procedure StampaScontrino to 'manager';

call RegistraUtente("ste","Stefano","Belli", "Cave", '1999-10-08',"Roma", "XXXXYYYYZZZZTTTT", "ste123", 1);

