drop database if exists pizzeriadb;
create database pizzeriadb;
use pizzeriadb;

-- passwd SHA1
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

create table Tavolo (
	NumTavolo smallint primary key,
	MaxCommensali tinyint unsigned not null,
	IsOccupato boolean not null default false
);

create table Ingrediente (
	Nome varchar(20) primary key,
	NumDisponibilitaScorte int unsigned not null,
	CostoAlKg float not null,
	check(
		CostoAlKg > 0
	)
);

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

create index ComposizioneProdotto_NomeProdotto_fk
	on ComposizioneProdotto(NomeProdotto asc);

create index ComposizioneProdotto_NomeIngrediente_fk
	on ComposizioneProdotto(NomeIngrediente asc);

create table Turno (
	DataOraInizio datetime primary key,
	DataOraFine datetime not null,
	check(
		DataOraFine > DataOraInizio
	),
	unique(
		DataOraFine
	)
);

create table TurnoAssegnato (
	Turno datetime 
		references Turno(DataOraInizio)
		on delete no action,
	Tavolo smallint
		references Tavolo(NumTavolo)
		on delete cascade,
	Cameriere varchar(10)
		references UtenteLavoratore(Username)
		on delete no action,
	primary key(
		Turno, 
		Tavolo, 
		Cameriere
	)
);

create index TurnoAssegnato_Turno_fk
	on TurnoAssegnato(Turno asc);

create index TurnoAssegnato_Tavolo_fk
	on TurnoAssegnato(Tavolo asc);

create index TurnoAssegnato_Cameriere_fk
	on TurnoAssegnato(Cameriere asc);

create table TavoloOccupato (
	DataOraOccupazione datetime primary key,
	Nome varchar(20) not null,
	Cognome varchar(20) not null,
	NumCommensali tinyint unsigned not null,
	IsServitoAlmenoUnaVolta boolean not null default false,
	Tavolo smallint not null 
		references Tavolo(NumTavolo)
		on delete cascade
);

create index TavoloOccupato_Tavolo_fk
	on TavoloOccupato(Tavolo asc);

create table Scontrino (
	IdFiscale int auto_increment primary key,
	DataOraEmissione datetime not null,
	CostoTotale float not null,
	IsPagato boolean not null default false,
	TavoloOccupato datetime
		 references TavoloOccupato(DataOraOccupazione)
		 on delete set null,
	check(
		CostoTotale > 0
	),
	unique(
		DataOraEmissione
	)
);

create index Scontrino_TavoloOccupato_fk
	on Scontrino(TavoloOccupato asc);

create table Ordinazione (
	TavoloOccupato datetime
		references TavoloOccupato(DataOraOccupazione)
		on delete cascade,
	NumOrdinazionePerTavolo tinyint,
	DataOraRichiesta datetime default NULL,
	Costo float unsigned not null default 0,
	DataOraCompletamento datetime default NULL,
	primary key (
			TavoloOccupato,
			NumOrdinazionePerTavolo
	),
	check(
		NumOrdinazionePerTavolo > 0 and 
			(DataOraRichiesta is NULL or 
				(DataOraRichiesta is not NULL and Costo > 0))
	)
);

create index Ordinazione_TavoloOccupato_fk
	on Ordinazione(TavoloOccupato asc);

create table SceltaDelCliente (
	TavoloOccupato datetime,
	NumOrdinazionePerTavolo tinyint,
	NumSceltaPerOrdinazione tinyint,
	DataOraCompletamento datetime default NULL,
	DataOraEspletata datetime default NULL,
	ProdottoNelMenu varchar(20)
		references ProdottoNelMenu(Nome)
		on delete set NULL,
	LavoratoreCucina varchar(10) default NULL
		references UtenteLavoratore(Username)
		on delete set default,
	foreign key (
		TavoloOccupato, 
		NumOrdinazionePerTavolo)
		references Ordinazione(
			TavoloOccupato, 
			NumOrdinazionePerTavolo
	)
	on delete cascade,
	primary key (
		TavoloOccupato,
		NumOrdinazionePerTavolo,
		NumSceltaPerOrdinazione
	),
	check(
		NumSceltaPerOrdinazione > 0
	)
);

create index SceltaDelCliente_ProdottoNelMenu_fk
	on SceltaDelCliente(ProdottoNelMenu asc);

create index SceltaDelCliente_LavoratoreCucina_fk
	on SceltaDelCliente(LavoratoreCucina asc);

create index 
	SceltaDelCliente_TavoloOccupato_NumOrdinazionePerTavolo_fk
	on SceltaDelCliente(TavoloOccupato asc, 
		NumOrdinazionePerTavolo asc);

create table AggiuntaAlProdotto (
	TavoloOccupato datetime,
	NumOrdinazionePerTavolo tinyint,
	NumSceltaPerOrdinazione tinyint,
	Ingrediente varchar(20)
		references Ingrediente(Nome)
		on delete cascade,
	QuantitaInGr float not null,
	foreign key (
		TavoloOccupato, 
		NumOrdinazionePerTavolo, 
		NumSceltaPerOrdinazione) 
		references SceltaDelCliente(
			TavoloOccupato,
			NumOrdinazionePerTavolo,
			NumSceltaPerOrdinazione
	)
	on delete cascade,
	primary key (
		TavoloOccupato, 
		NumOrdinazionePerTavolo, 
		NumSceltaPerOrdinazione, 
		Ingrediente
	),
	check(
		QuantitaInGr > 0
	)
);

create index AggiuntaAlProdotto_Ingrediente_fk on
	AggiuntaAlProdotto(Ingrediente asc);

create index 
	AggiuntaAlProdotto_TavoloOcc_NumOrdPerTa_NumSceltaPerOrd_fk
	on AggiuntaAlProdotto(TavoloOccupato asc, 
		NumOrdinazionePerTavolo asc,
		NumSceltaPerOrdinazione asc);

delimiter !

create trigger TurnoCheckDataOraInizio_Insert
before insert on Turno for each row
begin	
	if NEW.DataOraInizio < now() then
		signal sqlstate '45001' 
			set message_text = "Impossibile creare turno per il passato";
	end if;
end!

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
					O.Tavolo 
				from 
					Scontrino S join TavoloOccupato O on 
						O.DataOraOccupazione = S.TavoloOccupato 
				where
					S.TavoloOccupato = NEW.TavoloOccupato);
	end if;
end!

create trigger SceltaDelClienteCheckDates_BeforeUpdate
before update on SceltaDelCliente for each row
begin
	if NEW.DataOraCompletamento is not NULL and 
		NEW.DataOraEspletata is NULL then
		signal sqlstate '45007'
			set message_text = "Impossibile completare la scelta se prima non è stata espletata";
	end if;
end!

create trigger OrdinazioneCheckDates_BeforeUpdate
before update on Ordinazione for each row
begin
	if NEW.DataOraRichiesta is NULL and
		NEW.DataOraCompletamento is not NULL then
		signal sqlstate '45008'
			set message_text = "Impossibile completare l'ordinazione se prima non è stata chiusa";
	end if;
end!

create trigger OrdinazioneCheckAlreadyPendingOrScontrino_BeforeInsert
before insert on Ordinazione for each row
begin
	if exists(
		select 
			* 
		from 
			Ordinazione 
		where 
			TavoloOccupato = NEW.TavoloOccupato and 
			(DataOraRichiesta is NULL or DataOraCompletamento is NULL)) then
		
		signal sqlstate '45009'
			set message_text = "C'è un'ordinazione in attesa di completamento per il tavolo scelto";
	end if;

	if exists(
		select
			*
		from
			Scontrino
		where
			TavoloOccupato = NEW.TavoloOccupato) then

		signal sqlstate '45010'
			set message_text = "Lo scontrino per questo tavolo è già stato rilasciato";
	end if;
		
end!

create trigger SceltaDelClienteCheckDisp_BeforeInsert
before insert on SceltaDelCliente for each row
begin
	declare nomeIng varchar(20);
	declare cntDisp int;
	declare shouldLeave boolean;
	declare cur1 cursor for 
		select 
			Nome, NumDisponibilitaScorte
		from
			ComposizioneProdotto join Ingrediente on
				NomeIngrediente = Nome
		where
			NomeProdotto = NEW.ProdottoNelMenu;
	declare continue handler for not found set shouldLeave = true;

	set shouldLeave = false;

	open cur1;

	read_loop: loop
		fetch cur1 into nomeIng, cntDisp;
		
		if not shouldLeave then

			if cntDisp = 0 then
				leave read_loop;
			end if;

			update
				Ingrediente
			set
				NumDisponibilitaScorte = NumDisponibilitaScorte - 1
			where
				Nome = nomeIng;

		else

			leave read_loop;
		
		end if;

	end loop read_loop;

	close cur1;

	if cntDisp = 0 then
		signal sqlstate '45012'
			set message_text = "Uno o più ingredienti del prodotto non sono disponibili";
	end if;
end!

create trigger SceltaDelClienteAddCosto_AfterInsert
after insert on SceltaDelCliente for each row
begin
	update
		Ordinazione
	set
		Costo = Costo + (
							select
								CostoUnitario
							from
								ProdottoNelMenu
							where
								Nome = NEW.ProdottoNelMenu)
	where
		TavoloOccupato = NEW.TavoloOccupato and
		NumOrdinazionePerTavolo = NEW.NumOrdinazionePerTavolo;
end!

create trigger AggiuntaAlProdottoCheckDisp_BeforeInsert
before insert on AggiuntaAlProdotto for each row
begin
	declare numDispScorte int;

	select
		NumDisponibilitaScorte
	from
		Ingrediente
	where
		Nome = NEW.Ingrediente
	into
		numDispScorte;

	if numDispScorte = 0 then
		signal sqlstate '45013'
			set message_text = "L'ingrediente scelto non è disponibile";
	end if;

	update
		Ingrediente
	set
		NumDisponibilitaScorte = NumDisponibilitaScorte - 1
	where
		Nome = NEW.Ingrediente;
end!

create trigger AggiuntaAlProdottoAddCosto_AfterInsert
after insert on AggiuntaAlProdotto for each row
begin
	set @costoAlKg = (
		select 
			CostoAlKg 
		from 
			Ingrediente 
		where 
			Nome = NEW.Ingrediente);

	update
		Ordinazione
	set
		Costo = Costo + ((NEW.QuantitaInGr / 1000) * @costoAlKg)
	where
		TavoloOccupato = NEW.TavoloOccupato and
		NumOrdinazionePerTavolo = NEW.NumOrdinazionePerTavolo;
end!

create trigger SceltaDelClienteCheckTotalComplete_AfterUpdate
after update on SceltaDelCliente for each row
begin
	declare complDate datetime;
	declare shouldLeave boolean;
	declare cur1 cursor for select
								DataOraCompletamento
							from
								SceltaDelCliente
							where
								TavoloOccupato = NEW.TavoloOccupato and
								NumOrdinazionePerTavolo = NEW.NumOrdinazionePerTavolo;

	declare continue handler for not found set shouldLeave = true;

	set shouldLeave = false;

	open cur1;

	read_loop: loop

		fetch cur1 into complDate;

		if not shouldLeave then

			if complDate is NULL then
				leave read_loop;
			end if;

		else

			leave read_loop;

		end if;

	end loop read_loop;

	close cur1;

	if complDate is not NULL then
		update
			Ordinazione
		set
			DataOraCompletamento = now()
		where
			TavoloOccupato = NEW.TavoloOccupato and
			NumOrdinazionePerTavolo = NEW.NumOrdinazionePerTavolo;

		update
			TavoloOccupato
		set
			IsServitoAlmenoUnaVolta = true
		where
			DataOraOccupazione = NEW.TavoloOccupato;
	end if;
end!

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

create procedure AggiungiTurno(in inizio datetime, in fine datetime)
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

create procedure AssegnaTurno(
		in numTavolo smallint,
		in dataOraInizioTurno datetime, 
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

create function InTimeRange(monthly boolean, tm datetime)
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
		NumTavolo = @numTavoloAdatto;

	select @numTavoloAdatto as NumTavolo;

	commit;
end!

create function TutteOrdConcluse(dataOraOcc datetime)
returns boolean deterministic
begin
	declare numComplete int;
	declare numTotali int;

	select
		count(*)
	from
		Ordinazione
	where
		TavoloOccupato = dataOraOcc and
		DataOraRichiesta is not NULL and
		DataOraCompletamento is not NULL
	into
		numComplete;

	select
		count(*)
	from
		Ordinazione
	where
		TavoloOccupato = dataOraOcc
	into
		numTotali;

	return numTotali > 0 and numComplete > 0 and 
		numTotali = numComplete;
end!

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

	select
		Tavolo, DataOraOccupazione
	from
		TavoloOccupato
	where
		TutteOrdConcluse(DataOraOccupazione) and
		not exists (
			select
				*
			from
				Scontrino
			where
				TavoloOccupato = DataOraOccupazione);

	commit;
end!

create function CalcoloCostoTotale(dataOraOcc datetime)
returns float deterministic
begin
	declare costoTmp float;
	declare shouldLeave boolean;
	declare cur1 cursor for 
		select 
			Costo 
		from 
			Ordinazione 
		where 
			TavoloOccupato = dataOraOcc;
	declare continue handler for not found set shouldLeave = true;

	set shouldLeave = false;

	set @costoTotale = 0;

	open cur1;

	read_loop: loop
		fetch cur1 into costoTmp;

		if not shouldLeave then

			set @costoTotale = @costoTotale + costoTmp;

		else

			leave read_loop;

		end if;

	end loop read_loop;

	close cur1;

	return @costoTotale;
end!

create procedure StampaScontrino(in dataOraOcc datetime)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	if TutteOrdConcluse(dataOraOcc) = false then
		signal sqlstate '45014'
			set message_text = "Impossibile rilasciare lo scontrino. Esistono ordinazioni aperte.";
	end if;

	if exists (
				select
					* 
				from
					Scontrino
				where
					TavoloOccupato = dataOraOcc) then
		signal sqlstate '45006' 
			set message_text = "Scontrino già stampato";
	end if;

	insert into
		Scontrino(
			DataOraEmissione, 
			CostoTotale, 
			TavoloOccupato)
	values
		(now(), CalcoloCostoTotale(dataOraOcc), dataOraOcc);

	select
		Ord.NumOrdinazionePerTavolo as NumOrdinazione, 
		Sdc.NumSceltaPerOrdinazione as NumScelta,
		Sdc.ProdottoNelMenu as Prodotto,
		Ap.Ingrediente as IngredienteExtra,
		Ap.QuantitaInGr as QuantitaInGr
	from
		(Ordinazione Ord join SceltaDelCliente Sdc on
			Ord.TavoloOccupato = Sdc.TavoloOccupato and
			Ord.NumOrdinazionePerTavolo = Sdc.NumOrdinazionePerTavolo)
		left join AggiuntaAlProdotto Ap on
			Ord.TavoloOccupato = Ap.TavoloOccupato and
			Ord.NumOrdinazionePerTavolo = Ap.NumOrdinazionePerTavolo and
			Sdc.NumSceltaPerOrdinazione = Ap.NumSceltaPerOrdinazione
	where
		Ord.TavoloOccupato = dataOraOcc
	order by
		Ord.NumOrdinazionePerTavolo, 
		Sdc.NumSceltaPerOrdinazione,
		Sdc.ProdottoNelMenu asc;

	select
		IdFiscale, DataOraEmissione, CostoTotale
	from
		Scontrino
	where
		TavoloOccupato = dataOraOcc;

	commit;
end!

create procedure OttieniTavoliDiCompetenza(in username varchar(10))
begin
	set transaction read only;

	set @current_time = now();

	start transaction;
	
	select
		Ta.NumTavolo as NumTavolo, 
		Ta.IsOccupato as IsOccupato, 
		Tocc.DataOraOccupazione as DataOraOccupazione, 
		Tocc.NumCommensali as NumCommensali, 
		Tocc.IsServitoAlmenoUnaVolta as IsServitoAlmenoUnaVolta
	from
		((Tavolo Ta left join TavoloOccupato Tocc on
			Ta.NumTavolo = Tocc.Tavolo) join TurnoAssegnato Tu on
				Tu.Tavolo = Ta.NumTavolo) join Turno T on
					T.DataOraInizio = Tu.Turno
	where
		Tu.Cameriere = username and
		T.DataOraInizio <= @current_time and
		@current_time <= T.DataOraFine and
		Ta.IsOccupato = true and
		not exists (
			select
				*
			from
				Scontrino
			where
				TavoloOccupato = Tocc.DataOraOccupazione and
				IsPagato = true);
		
	commit;
end!

create procedure OttieniTavoliAssegnati(in username varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	set @current_time = now();

	start transaction;

	select
		Ta.NumTavolo as NumTavolo
	from
		(Tavolo Ta join TurnoAssegnato Tu on
			Ta.NumTavolo = Tu.Tavolo) join Turno T on
				Tu.Turno = T.DataOraInizio
	where
		Tu.Cameriere = username and
		T.DataOraInizio <= @current_time and
		@current_time <= T.DataOraFine;

	commit;
end!

create function CanWorkOnTable(dataOraOcc datetime, usern varchar(10))
returns boolean deterministic
begin
	set @current_time = now();

	if exists(
		select
			*
		from
			(TavoloOccupato Tocc join Tavolo Ta on
				Tocc.Tavolo = Ta.NumTavolo) join TurnoAssegnato TuAs on
					TuAs.Tavolo = Ta.NumTavolo join Turno Tu on
						Tu.DataOraInizio = TuAs.Turno and
						Tu.DataOraInizio <= @current_time and
						@current_time <= Tu.DataOraFine
		where
			TuAs.Cameriere = usern and 
			Tocc.DataOraOccupazione = dataOraOcc) then
			return true;
	end if;

	return false;
end!

create procedure PrendiOrdinazione(in dataOraOcc datetime, in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	if CanWorkOnTable(dataOraOcc, usern) then
		set @counter = (select 
							count(*) 
						from 
							Ordinazione 
						where
							TavoloOccupato = dataOraOcc) + 1;

		insert into
			Ordinazione(TavoloOccupato, NumOrdinazionePerTavolo)
		values
			(dataOraOcc, @counter);
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure ChiudiOrdinazione(in dataOraOcc datetime, in usern varchar(20))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;
	
	if CanWorkOnTable(dataOraOcc, usern) then
		set @numOrd =(
						select 
							count(*) 
						from 
							Ordinazione 
						where
							TavoloOccupato = dataOraOcc
		);

		update
			Ordinazione
		set
			DataOraRichiesta = now()
		where
			TavoloOccupato = dataOraOcc and
			DataOraRichiesta is NULL and
			DataOraCompletamento is NULL and
			NumOrdinazionePerTavolo = @numOrd;
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure PrendiSceltaPerOrd(
	in dataOraOcc datetime, 
	in nomeProd varchar(20), 
	in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	if CanWorkOnTable(dataOraOcc, usern) then
		set @numOrd = (
			select
				NumOrdinazionePerTavolo
			from
				Ordinazione
			where
				TavoloOccupato = dataOraOcc and
				DataOraRichiesta is NULL);

		if @numOrd is NULL then
			signal sqlstate '45015'
				set message_text = "Ordinazione chiusa";
		end if;

		set @numSc = (select 
							count(*) 
						from 
							SceltaDelCliente 
						where 
							TavoloOccupato = dataOraOcc and 
							NumOrdinazionePerTavolo = @numOrd) + 1;
		insert into 
			SceltaDelCliente(TavoloOccupato, 
							NumOrdinazionePerTavolo, 
							NumSceltaPerOrdinazione, 
							ProdottoNelMenu)
		values(
			dataOraOcc,
			@numOrd,
			@numSc,
			nomeProd);
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure OttieniSceltePerOrdinazione(in dataOraOcc datetime, in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	if CanWorkOnTable(dataOraOcc, usern) then
		select
			Sdc.NumOrdinazionePerTavolo as NumOrdinazionePerTavolo,
			Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrdinazione, 
			Sdc.ProdottoNelMenu as ProdottoNelMenu
		from
			SceltaDelCliente Sdc join Ordinazione Ord on
				Sdc.TavoloOccupato = Ord.TavoloOccupato and
				Sdc.NumOrdinazionePerTavolo = Ord.NumOrdinazionePerTavolo
		where
			Sdc.TavoloOccupato = dataOraOcc and
			Ord.DataOraRichiesta is NULL;
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure AggiungiIngExtraAllaScelta(
	in dataOraOcc datetime,
	in numOrdPerTav tinyint,
	in numSceltaPerOrd tinyint,
	in ing varchar(20),
	in qtGr float,
	in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	if CanWorkOnTable(dataOraOcc, usern) then
		insert into
			AggiuntaAlProdotto(
				TavoloOccupato,
				NumOrdinazionePerTavolo,
				NumSceltaPerOrdinazione,
				Ingrediente,
				QuantitaInGr)
		values
			(
				dataOraOcc,
				numOrdPerTav,
				numSceltaPerOrd,
				ing,
				qtGr
			);
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure OttieniScelteEspletate(in usern varchar(10))
begin
	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	select
		TavoloOccupato,
		NumOrdinazionePerTavolo,
		NumSceltaPerOrdinazione,
		Tavolo,
		ProdottoNelMenu
	from
		SceltaDelCliente join TavoloOccupato on
			TavoloOccupato = DataOraOccupazione
	where
		CanWorkOnTable(TavoloOccupato, usern) and
		DataOraEspletata is not NULL and
		DataOraCompletamento is NULL;
	
	commit;
end!

create procedure EffettuaConsegna(
	in dataOraOcc datetime, 
	in numOrd tinyint, 
	in numScelta tinyint, 
	in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	if CanWorkOnTable(dataOraOcc, usern) then
		update
			SceltaDelCliente
		set
			DataOraCompletamento = now()
		where
			TavoloOccupato = dataOraOcc and
			NumOrdinazionePerTavolo = numOrd and
			NumSceltaPerOrdinazione = numScelta;
	else
		signal sqlstate '45011'
			set message_text = "Non è possibile selezionare il tavolo (ad esempio, non è occupato al momento)";
	end if;

	commit;
end!

create procedure OttieniScelteDaPreparare()
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	select
		Sdc.TavoloOccupato as TavoloOccupato,
		Tocc.Tavolo as NumTavolo,
		Sdc.NumOrdinazionePerTavolo as NumOrdinazionePerTavolo,
		Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrdinazione,
		Sdc.ProdottoNelMenu as ProdottoNelMenu
	from
		(SceltaDelCliente Sdc join Ordinazione Ord on
			Sdc.TavoloOccupato = Ord.TavoloOccupato and
			Sdc.NumOrdinazionePerTavolo = Ord.NumOrdinazionePerTavolo) join TavoloOccupato Tocc on 
				Sdc.TavoloOccupato = Tocc.DataOraOccupazione
	where
		Sdc.LavoratoreCucina is NULL and
		Sdc.DataOraEspletata is NULL and
		Sdc.DataOraCompletamento is NULL and
		Ord.DataOraCompletamento is NULL and
		Ord.DataOraRichiesta is not NULL;

	commit;
end!

create procedure PrendiInCaricoScelta(
	in dataOraOcc datetime,
	in numOrd int,
	in numSc int,
	in nomeProd varchar(20),
	in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	set @role = (
		select
			Ruolo
		from
			UtenteLavoratore
		where
			Username = usern
	);

	set @isBarMenu = (
		select
			IsBarMenu
		from
			ProdottoNelMenu
		where
			Nome = nomeProd	
	);

	if @isBarMenu = true and @role <> 4 then
		signal sqlstate '45016'
			set message_text = "Non sei un barista!";
	elseif @isBarMenu = false and @role <> 3 then
		signal sqlstate '45017'
			set message_text = "Non sei un pizzaiolo!";
	end if;

	update
		SceltaDelCliente
	set
		LavoratoreCucina = usern
	where
		TavoloOccupato = dataOraOcc and
		NumOrdinazionePerTavolo = numOrd and
		NumSceltaPerOrdinazione = numSc and
		LavoratoreCucina is NULL;

	commit;
end!

create procedure OttieniSceltePreseInCaricoNonEspletate(in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	select
		Sdc.TavoloOccupato as TavoloOccupato,
		Tocc.Tavolo as NumTavolo,
		Sdc.NumOrdinazionePerTavolo as NumOrdinazionePerTavolo,
		Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrdinazione,
		Sdc.ProdottoNelMenu as ProdottoNelMenu
	from
		SceltaDelCliente Sdc join TavoloOccupato Tocc on
			Sdc.TavoloOccupato = Tocc.DataOraOccupazione 
	where
		Sdc.DataOraEspletata is NULL and
		Sdc.LavoratoreCucina = usern;

	commit;
end!

create procedure EspletaSceltaPresaInCarico(
	in dataOraOcc datetime,
	in numOrd int,
	in numSc int,
	in usern varchar(10)
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;

	start transaction;

	update
		SceltaDelCliente
	set
		DataOraEspletata = now()
	where
		TavoloOccupato = dataOraOcc and
		NumOrdinazionePerTavolo = numOrd and
		NumSceltaPerOrdinazione = numSc and
		DataOraEspletata is NULL and
		LavoratoreCucina = usern;

	commit;
end!

create procedure OttieniInfoProdottiDiScelteInCarico(in usern varchar(10))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction read only;
	set transaction isolation level read committed;

	start transaction;

	select
		Tocc.Tavolo as Tavolo,
		Sdc.NumOrdinazionePerTavolo as NumOrdPerTavolo,
		Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrd,
		Sdc.ProdottoNelMenu as Prodotto,
		Ap.Ingrediente as IngredienteExtra,
		Ap.QuantitaInGr as QuantitaIngredienteExtraInGr
	from
		((SceltaDelCliente Sdc join ProdottoNelMenu Prod on
			Sdc.ProdottoNelMenu = Prod.Nome) left join AggiuntaAlProdotto Ap on
				Ap.TavoloOccupato = Sdc.TavoloOccupato and
				Ap.NumOrdinazionePerTavolo = Sdc.NumOrdinazionePerTavolo and
				Ap.NumSceltaPerOrdinazione = Sdc.NumSceltaPerOrdinazione) join TavoloOccupato Tocc on
					Sdc.TavoloOccupato = Tocc.DataOraOccupazione
	where
		Sdc.LavoratoreCucina = usern and
		Sdc.DataOraEspletata is NULL;

	commit;
end!

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

grant execute on procedure OttieniTavoliDiCompetenza to 'cameriere';
grant execute on procedure PrendiOrdinazione to 'cameriere';
grant execute on procedure ChiudiOrdinazione to 'cameriere';
grant execute on procedure PrendiSceltaPerOrd to 'cameriere';
grant execute on procedure OttieniSceltePerOrdinazione to 'cameriere';
grant execute on procedure AggiungiIngExtraAllaScelta to 'cameriere';
grant execute on procedure OttieniScelteEspletate to 'cameriere';
grant execute on procedure EffettuaConsegna to 'cameriere';
grant execute on procedure OttieniTavoliAssegnati to 'cameriere';

grant execute on procedure OttieniScelteDaPreparare to 'pizzaiolo';
grant execute on procedure PrendiInCaricoScelta to 'pizzaiolo';
grant execute on procedure OttieniSceltePreseInCaricoNonEspletate to 'pizzaiolo';
grant execute on procedure EspletaSceltaPresaInCarico to 'pizzaiolo';
grant execute on procedure OttieniInfoProdottiDiScelteInCarico to 'pizzaiolo';

grant execute on procedure OttieniScelteDaPreparare to 'barman';
grant execute on procedure PrendiInCaricoScelta to 'barman';
grant execute on procedure OttieniSceltePreseInCaricoNonEspletate to 'barman';
grant execute on procedure EspletaSceltaPresaInCarico to 'barman';
grant execute on procedure OttieniInfoProdottiDiScelteInCarico to 'barman';

-- fine

call RegistraUtente("ste","Stefano","Belli", "Cave", '1999-10-08',"Roma", "XXXXYYYYZZZZTTTT", "ste123", 1);
call RegistraUtente("pippo", "Pippo", "Baudo", "Roma", '2002-01-10', "Milano", "ABCDEFGHILMN0123", "tanto", 2);
call RegistraUtente("titti", "Titti", "Boh", "Milano", '2003-01-20', "Torino", "ZZZZYYYYTTTTXXXX", "zitti", 3);
call RegistraUtente("lollo", "Lollo", "Boh", "Milano", '2003-01-21', "Torino", "ZZZZYYYYTTTTXXXZ", "culo", 4);
