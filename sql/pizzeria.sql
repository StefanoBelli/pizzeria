--
-- TODO
--
-- * scontrino
-- * conteggio scorte
--
-- * Needed consistency checks
-- * where # select > 2 => Optimize if possible
-- * Functional testing
--
-- * Indexes
-- * Transaction Isolation
-- * Integ. ref. on update/delete
--

create database pizzeriadb;
use pizzeriadb;

create table Lavoratore(
	Username varchar(10) primary key,
	Nome varchar(20) not null,
	Cognome varchar(20) not null,
	ComuneResidenza varchar(34) not null,
	DataNascita date not null,
	ComuneNascita varchar(34) not null,
	CF char(16) not null,
	IsManager boolean not null,
	
	unique(
		CF
	) 
);

create table Cameriere(
	Lavoratore varchar(10) primary key references Lavoratore(Username)
);

create table LavoratoreCucina(
	Lavoratore varchar(10) primary key references Lavoratore(Username),
	IsBarman boolean not null
);

create table Turno(
	DataOraInizio timestamp primary key,
	DataOraFine timestamp not null,
	
	check(
		DataOraFine > DataOraInizio
	),

	unique(
		DataOraFine
	)
);

create table Tavolo(
	NumTavolo smallint primary key,
	NumMaxCommensali smallint not null,
	IsOccupato boolean not null
);

create table TurnoAssegnato(
	Turno timestamp references Turno(DataOraInizio),
	Tavolo smallint references Tavolo(NumTavolo),
	Cameriere varchar(10) references Cameriere(Lavoratore),
	primary key (Turno, Tavolo, Cameriere)
);

create table Scontrino(
	IdFiscale int primary key,
	DataOraEmissione timestamp not null,
	CostoTotale float not null

	check(
		CostoTotale > 0
	),
	
	unique(
		DataOraEmissione
	)
);

create table TavoloOccupato(
	DataOraOccupazione timestamp primary key,
	Nome varchar(20) not null,
	Cognome varchar(20) not null,
	NumCommensali smallint not null,
	IsServitoAlmenoUnaVolta boolean not null,
	IsPagato boolean not null,
	Tavolo smallint not null references Tavolo(NumTavolo),
	Scontrino int references Scontrino(IdFiscale),

	check(
		(Scontrino is NULL and IsPagato = False)
		or (Scontrino is not NULL and (IsPagato = False or IsPagato = True))
	)
);

create table Ordinazione(
	TavoloOccupato timestamp references TavoloOccupato(DataOraOccupazione),
	NumOrdinazionePerTavolo smallint not null,
	DataOraRichiesta timestamp,
	Costo float unsigned not null default 0,
	DataOraCompletamento timestamp,
	primary key(TavoloOccupato, NumOrdinazionePerTavolo),

	unique(
		DataOraRichiesta
	)
);

create table ProdottoNelMenu(
	Nome varchar(20) primary key,
	CostoUnitario float not null,
	IsAlcolico boolean,
	IsBarMenu boolean not null,

	check(
		CostoUnitario > 0 and (
			(IsAlcolico is not NULL and IsBarMenu is True) or
			(IsAlcolico is NULL and IsBarMenu is False))
	)
);

create table Ingrediente(
	Nome varchar(20) primary key,
	NumDisponibilitaScorte int not null,
	CostoAlKg float not null,
	
	check(
		CostoAlKg > 0 and NumDisponibilitaScorte >= 0
	)
);

create table ComposizioneDiBase(
	ProdottoNelMenu varchar(20) references ProdottoNelMenu(Nome) 
		on delete cascade,
	Ingrediente varchar(20) references Ingrediente(Nome)
		on delete restrict,
	primary key (ProdottoNelMenu, Ingrediente)
);

create table SceltaDelCliente(
	TavoloOccupato timestamp,
	NumOrdinazionePerTavolo smallint,
	NumSceltaPerOrdinazione smallint,
	DataOraCompletamento timestamp,
	DataOraEspletata timestamp,
	ProdottoNelMenu varchar(20) not null references ProdottoNelMenu(Nome),
	LavoratoreCucinaInCarico varchar(10) 
		references LavoratoreCucina(Lavoratore),
	foreign key (TavoloOccupato,NumOrdinazionePerTavolo)
		references Ordinazione(TavoloOccupato, NumOrdinazionePerTavolo),
	primary key (TavoloOccupato, NumOrdinazionePerTavolo, 
		NumSceltaPerOrdinazione),
	
	check(
		(DataOraEspletata is not NULL and DataOraCompletamento is not NULL) 
		or (DataOraEspletata is not NULL and DataOraCompletamento is NULL) 
		or (DataOraCompletamento is NULL and DataOraEspletata is NULL)
	)
);

create table AggiuntaAlProdotto(
	TavoloOccupato timestamp,
	NumOrdinazionePerTavolo smallint,
	NumSceltaPerOrdinazione smallint,
	Ingrediente varchar(20) references Ingrediente(Nome),
	QuantitaInGr float not null,
	foreign key (
		TavoloOccupato, 
		NumOrdinazionePerTavolo, 
		NumSceltaPerOrdinazione) references SceltaDelCliente(
			TavoloOccupato, 
			NumOrdinazionePerTavolo, 
			NumSceltaPerOrdinazione),
	primary key(
		TavoloOccupato, 
		NumOrdinazionePerTavolo, 
		NumSceltaPerOrdinazione, 
		Ingrediente),
	check(
		QuantitaInGr > 0
	)
);

set autocommit = 0;

delimiter !

create function IsInBetween(
	lowerBound int, 
	upperBound int, 
	val int)
returns boolean
deterministic
begin
	return (val >= lowerBound and val <= upperBound);
end!

create function GetMyUsername()
returns varchar(10)
deterministic
begin
	return (substring_index(user(), '@', 1));
end!

create procedure GetMyUserRole()
begin
	set @myUsername = (select GetMyUsername());
	set @isManager = (
		select Username 
		from Lavoratore 
		where Username = @myUsername and IsManager = true);
	
	if found_rows() = 1 then
		select 1 as Role;
	else
		set @isCameriere = (
			select Lavoratore
			from Cameriere 
			where Lavoratore = @myUsername);
			
		if found_rows() = 1 then
			select 2 as Role;
		else
			set @isLavoratoreCucina = (
				select Lavoratore
				from LavoratoreCucina 
				where Lavoratore = @myUsername);
				
			if found_rows() = 1 then
				select 3 as Role;
			else
				select 4 as Role;
			end if;
		end if;
	end if;
end!

create procedure CreateUser_Internal(
	in username varchar(10), 
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	set @sql := concat("create user ", username, "@'%' identified by '", password,"';");
	prepare stmt from @sql;
	execute stmt;
	deallocate prepare stmt;
end!

create procedure GivePrivOnResToUser_Internal(
	in username varchar(10),
	in singleres varchar(50),
	in grantOpt boolean) 
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	set @sql := concat("grant execute on procedure ", singleres, " to ", username);
	if grantOpt = true then
		set @sql := concat(@sql, " with grant option;");
	else
		set @sql := concat(@sql, ";");
	end if;

	prepare stmt from @sql;
	execute stmt;
	deallocate prepare stmt;
end!

create procedure Insertion_Lavoratore_Internal(
	in nome varchar(20), 
	in cognome varchar(20), 
	in comuneResidenza varchar(34), 
	in dataNascita date, 
	in comuneNascita varchar(34), 
	in cf char(16),
	in username varchar(10), 
	in isManager boolean)
begin
	declare exit handler for sqlexception
	begin
		resignal; -- raise again the sql exception to the caller
	end;

	insert into Lavoratore(
		Nome, Cognome, ComuneResidenza, 
		DataNascita, ComuneNascita, CF, 
		Username, IsManager) values(nome, cognome, 
		comuneResidenza, dataNascita, comuneNascita, 
		cf, username, isManager);
end!

create procedure Insertion_LavoratoreCucina_Internal(
	in usernameLavoratore varchar(10),
	in isBarman boolean)
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	insert into LavoratoreCucina(Lavoratore, IsBarman) 
		values(usernameLavoratore, isBarman);
end!

create procedure Insertion_Cameriere_Internal(
	in usernameLavoratore varchar(10))
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	insert into Cameriere(Lavoratore) 
		values(usernameLavoratore);
end!

create procedure CreaManager(
	in nome varchar(20), 
	in cognome varchar(20), 
	in comuneResidenza varchar(34), 
	in dataNascita date, 
	in comuneNascita varchar(34), 
	in cf char(16),
	in username varchar(10),
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, true);
	
	call CreateUser_Internal(username, password);
	
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaManager", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaPizzaiolo", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaBarman", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaCameriere", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.OttieniTavoloDisponibile", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaTurno", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.AssegnaTurno", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.AggiungiProdottoBar", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.AggiungiProdottoPizzeria", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaIngrediente", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaComposizione", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.TogliDalMenu", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaTavolo", true);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.GetMyUserRole", true);
	
	commit;
end!

create procedure CreaPizzaiolo(
	in nome varchar(20), 
	in cognome varchar(20), 
	in comuneResidenza varchar(34), 
	in dataNascita date, 
	in comuneNascita varchar(34), 
	in cf char(16),
	in username varchar(10),
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_LavoratoreCucina_Internal(username, false);

	call CreateUser_Internal(username, password);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.SituazioneOrdini", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.PrendiInCaricoOrdine", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.EspletaOrdine", false);
	
	commit;
end!

create procedure CreaBarman(
	in nome varchar(20), 
	in cognome varchar(20), 
	in comuneResidenza varchar(34), 
	in dataNascita date, 
	in comuneNascita varchar(34), 
	in cf char(16),
	in username varchar(10),
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal; 
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_LavoratoreCucina_Internal(username, true);

	call CreateUser_Internal(username, password);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.SituazioneOrdini", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.PrendiInCaricoOrdine", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.EspletaOrdine", false);
	

	commit;
end!

create procedure CreaCameriere(
	in nome varchar(20), 
	in cognome varchar(20), 
	in comuneResidenza varchar(34), 
	in dataNascita date, 
	in comuneNascita varchar(34), 
	in cf char(16),
	in username varchar(10),
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_Cameriere_Internal(username);

	call CreateUser_Internal(username, password);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.SituazioneTavoliAssegnati", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.SituazioneOrdinazioniDaConsegnare", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.CreaNuovaOrdinazionePerTavolo", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.ChiudiOrdinazionePerTavolo", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.AggiungiSceltaAllOrdCorrentePerTavolo", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.AggiungiIngredienteAllaScelta", false);
	call GivePrivOnResToUser_Internal(username, "pizzeriadb.ConsegnaOrdinazione", false);

	commit;
end!

-- Cameriere

create procedure SituazioneTavoliAssegnati()
begin
	select 
		Ta.NumTavolo as NumTavolo, 
		Toc.Cognome as Cognome,
		Toc.IsServitoAlmenoUnaVolta as IsServitoAlmenoUnaVolta,
		Toc.DataOraOccupazione as DataOraOccupazione
	from 
		Tavolo Ta left join TavoloOccupato Toc on 
			Ta.NumTavolo = Toc.Tavolo and 
			Toc.Scontrino is NULL
	where
		Toc.Tavolo = any (
			select 
				Tavolo
			from 
				TurnoAssegnato Tas join Turno Tur on 
					Tas.Turno = Tur.DataOraInizio
			where 
				Tas.Cameriere = GetMyUsername() and 
					IsInBetween(Tur.DataOraInizio, Tur.DataOraFine, now()));

	commit;
end!

create procedure SituazioneOrdinazioniDaConsegnare()
begin
	select 
		Tocc.NumTavolo as NumTavolo, 
		Tocc.Cognome as Cognome, 
		Sdc.NumOrdinazionePerTavolo as NumOrdinazionePerTavolo, 
		Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrdinazione, 
		Sdc.DataOraEspletata as DataOraEspletata,
		Tocc.DataOraOccupazione as DataOraOccupazione
	from 
		SceltaDelCliente Sdc join Ordinazione Ord on
			Sdc.TavoloOccupato = Ord.TavoloOccupato and
			Sdc.NumOrdinazionePerTavolo = Ord.NumOrdinazionePerTavolo
		join TavoloOccupato Tocc on 
			Tocc.DataOraOccupazione = Ord.TavoloOccupato and 
			Tocc.Scontrino is NULL
	where 
		Ord.DataOraRichista is not NULL and 
		Ord.DataOraCompletamento is NULL and
		Sdc.DataOraCompletamento is NULL and
		Tocc.Tavolo = any (
			select 
				Tur.Tavolo
			from 
				TurnoAssegnato Tas join Turno Tur on 
					Tas.Turno = Tur.DataOraInizio
			where 
				Tas.Cameriere = GetMyUsername() and 
					IsInBetween(Tur.DataOraInizio, Tur.DataOraFine, now()));
	
	commit;
end!

create trigger SoloUnaOrdinazioneInPending
before insert on Ordinazione for each row
begin
	declare counter int;

	select 
		count(*)
	from
		Ordinazione Ord join TavoloOccupato Tocc on 
			Tocc.DataOraOccupazione = Ord.TavoloOccupato 
	where 
		Ord.DataOraRichiesta is NULL and Tocc.NumTavolo = any (
			select 
				Tur.Tavolo
			from 
				TurnoAssegnato Tas join Turno Tur on 
					Tas.Turno = Tur.DataOraInizio
			where 
				Tas.Cameriere = GetMyUsername() and 
					IsInBetween(Tur.DataOraInizio, Tur.DataOraFine, now())) into counter;

	if counter > 0 then
		signal sqlstate '45000';

	end if;
end!

create procedure CreaNuovaOrdinazionePerTavolo(
	in tavoloOccupato timestamp
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @numOrd = (
		select 
			max(Ord.NumOrdinazionePerTavolo)
		from 
			Ordinazione Ord join TavoloOccupato Tocc on 
				Ord.TavoloOccupato = Tocc.DataOraOccupazione
		where
			Ord.TavoloOccupato = @tavoloOccupato
	) + 1;

	insert into Ordinazione(TavoloOccupato, NumOrdinazionePerTavolo)
		values(tavoloOccupato, @numOrd);

	select @numOrd;

	commit;
end!

create trigger NoOrdinazioneInPending
before update on Ordinazione for each row
begin
	declare counter int;

	select 
		count(*)
	from
		Ordinazione Ord join TavoloOccupato Tocc on 
			Tocc.DataOraOccupazione = Ord.TavoloOccupato 
	where 
		Ord.DataOraRichiesta is NULL and Tocc.NumTavolo = any (
			select 
				Tur.Tavolo
			from 
				TurnoAssegnato Tas join Turno Tur on 
					Tas.Turno = Tur.DataOraInizio
			where 
				Tas.Cameriere = GetMyUsername() and 
					IsInBetween(Tur.DataOraInizio, Tur.DataOraFine, now())) into counter;

	if counter = 0 then
		signal sqlstate '45000';

	end if;
end!

create procedure ChiudiOrdinazionePerTavolo()
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	update 
		Ordinazione Ord
	set 
		Ord.DataOraOccupazione = now()
	where 
		(Ord.TavoloOccupato, Ord.NumOrdinazionePerTavolo) = (
			select 
				TavoloOccupato, NumOrdinazionePerTavolo
			from 
				Ordinazione Ord join TavoloOccupato Tocc on 
					Ord.TavoloOccupato = Tocc.DataOraOccupazione join Tavolo Tav on
						Tav.NumTavolo = Tocc.Tavolo 
			where
				Tocc.Scontrino is NULL and Ord.DataOraRichiesta is NULL and Tav.NumTavolo = (
					select 
						Tur.Tavolo
					from 
						TurnoAssegnato Tas join Turno Tur on 
							Tas.Turno = Tur.DataOraInizio
					where 
						Tas.Cameriere = GetMyUsername() and 
							IsInBetween(Tur.DataOraInizio, Tur.DataOraFine, now())));

	commit;
end!

create procedure AggiungiSceltaAllOrdCorrentePerTavolo(
	in tavoloOccupato timestamp,
	in ordPerTavolo smallint,
	in nomeProdottoMenu varchar(20)
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @numSc = (
		select
			max(Sdc.NumSceltaPerOrdinazione)
		from
			Ordinazione Ord join TavoloOccupato Tocc on
				Ord.TavoloOccupato = Tocc.DataOraOccupazione
			join SceltaDelCliente Sdc on
				Sdc.TavoloOccupato = Ord.TavoloOccupato and
				Sdc.NumOrdinazionePerTavolo = Ord.NumOrdinazionePerTavolo
		where
			Sdc.TavoloOccupato = @tavoloOccupato and 
			Sdc.NumOrdinazionePerTavolo = @numOrd
	) + 1;
	
	-- checks
	-- diminuisci scorte
 
	insert into SceltaDelCliente(TavoloOccupato, NumOrdinazionePerTavolo, NumSceltaPerOrdinazione, ProdottoNelMenu)
		values(tavoloOccupato, ordPerTavolo, @numSc, nomeProdottoMenu);

	select @numSc;

	commit;
end!

create procedure AggiungiIngredienteAllaScelta(
	in tavoloOccupato timestamp,
	in ordPerTavolo smallint,
	in sceltaPerOrd smallint,
	in ingredienteAggiuntivo varchar(20),
	in quantitaInGr float
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	-- checks
	-- diminuisci scorte

	insert into AggiuntaAlProdotto(TavoloOccupato, NumOrdinazionePerTavolo, NumSceltaPerOrdinazione, Ingrediente, QuantitaInGr)
		values(tavoloOccupato, ordPerTavolo, sceltaPerOrd, ingredienteAggiuntivo, quantitaInGr);

	commit;
end!

create procedure ConsegnaOrdinazione(
	in tavoloOccupato timestamp,
	in ordPerTavolo smallint,
	in sceltaPerOrd smallint
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @nowTime = now();

	-- checks
	-- aggiunta al conto

	update
		SceltaDelCliente
	set
		DataOraCompletamento = @nowTime
	where
		TavoloOccupato = tavoloOccupato and
		NumOrdinazionePerTavolo = ordPerTavolo and
		NumSceltaPerOrdinazione = sceltaPerOrd and
		DataOraCompletamento is NULL and
		DataOraEspletata is not NULL;

	set @isTutteConsegnate = (
			select
				count(*) = max(NumSceltaPerOrdinazione)
			from
				SceltaDelCliente
			where
				TavoloOccupato = tavoloOccupato and
				NumOrdinazionePerTavolo = ordPerTavolo and
				NumSceltaPerOrdinazione = sceltaPerOrd and
				DataOraCompletamento is not NULL);
	
	if @isTutteConsegnate = true then
		update
			Ordinazione
		set
			DataOraCompletamento = @nowTime
		where
			TavoloOccupato = tavoloOccupato and
			NumOrdinazionePerTavolo = ordPerTavolo and
			DataOraCompletamento is NULL;
	end if;

	commit;
end!

-- Pizzaiolo e barman

create procedure SituazioneOrdini(
	in inAttesaDiPresaInCarico boolean
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @sql = concat("
		select 
			Sdc.TavoloOccupato as TavoloOccupato,
			Sdc.NumOrdinazionePerTavolo as NumOrdinazionePerTavolo,
			Sdc.NumSceltaPerOrdinazione as NumSceltaPerOrdinazione,
			Sdc.ProdottoNelMenu as ProdottoNelMenu,
			Ap.Ingrediente as Ingrediente,
			Ap.QuantitaInGr as QuantitaInGr
		from 
			SceltaDelCliente Sdc left join AggiuntaAlProdotto Ap on
			Sdc.TavoloOccupato = Ap.TavoloOccupato and
			Sdc.NumOrdinazionePerTavolo = Ap.NumOrdinazionePerTavolo and
			Sdc.NumSceltaPerOrdinazione = Ap.NumSceltaPerOrdinazione");

	if inAttesaDiPresaInCarico = true then
		set @sql = concat(@sql, "
			where
				Sdc.LavoratoreCucinaInCarico is NULL;");
	else
		set @sql = concat(@sql, "
			where
				Sdc.LavoratoreCucinaInCarico = GetMyUsername();");
	end if;

	prepare stmt from @sql;
	execute stmt;
	deallocate prepare stmt;

	commit;
end!

create procedure PrendiInCaricoOrdine(
	in tavoloOccupato timestamp,
	in numOrdinazionePerTavolo smallint,
	in numSceltaPerOrdinazione smallint
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @myUsername = GetMyUsername();

	set @isBarMenu = (
		select 
			Pm.IsBarMenu
		from
			SceltaDelCliente Sdc join ProdottoNelMenu Pm on 
				Sdc.ProdottoNelMenu = Pm.Nome
		where
			Sdc.TavoloOccupato = tavoloOccupato and
			Sdc.NumOrdinazionePerTavolo = numOrdinazionePerTavolo and
			Sdc.NumSceltaPerOrdinazione = numSceltaPerOrdinazione);
		
	set @isBarman = (
		select
			IsBarman
		from
			LavoratoreCucina Lc
		where
			Lc.Lavoratore = @myUsername);

	if @isBarman = @isBarMenu then
		update 
			SceltaDelCliente Sdc
		set
			Sdc.LavoratoreCucinaInCarico = @myUsername
		where
			Sdc.LavoratoreCucinaInCarico is NULL and
			Sdc.TavoloOccupato = tavoloOccupato and
			Sdc.NumOrdinazionePerTavolo = numOrdinazionePerTavolo and
			Sdc.NumSceltaPerOrdinazione = numSceltaPerOrdinazione;
	else
		signal sqlstate '45000';
	end if;

	commit;
end!

create procedure EspletaOrdine(
	in tavoloOccupato timestamp,
	in numOrdinazionePerTavolo smallint,
	in numSceltaPerOrdinazione smallint
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	update
		SceltaDelCliente Sdc
	set
		Sdc.DataOraEspletata = now()
	where
		Sdc.LavoratoreCucinaInCarico = GetMyUsername() and
		Sdc.TavoloOccupato = tavoloOccupato and
		Sdc.NumOrdinazionePerTavolo = numOrdinazionePerTavolo and
		Sdc.NumSceltaPerOrdinazione = numSceltaPerOrdinazione;

	commit;
end!

-- Manager

create procedure OttieniTavoloDisponibile(
	in numTavolo smallint,
	in numCommensali smallint,
	in nome varchar(20),
	in cognome varchar(20)
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set @rightNow = now();

	set @numTavoloTrovato = (select NumTavolo
		from Tavolo T
		where T.IsOccupato = false 
			and T.NumMaxCommensali <= numCommensali 
			and T.NumTavolo = any (
				select Tavolo
				from Turno Tu
				where IsBetween(Tu.DataOraInizio, Tu.DataOraFine, @rightNow)));

	insert into TavoloOccupato(
		DataOraOccupazione, 
		Nome, 
		Cognome, 
		NumCommensali, 
		IsServitoAlmenoUnaVolta, 
		IsPagato, 
		Tavolo)
	values(
		@rightNow,
		nome,
		cognome,
		numCommensali,
		false,
		false,
		@numTavoloTrovato
	);

	select @numTavoloTrovato;

	commit;
end!

create procedure CreaTurno(
	in dataOraInizio timestamp,
	in dataOraFine timestamp)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into Turno(DataOraInizio, DataOraFine)
		values(dataOraInizio, dataOraFine);

	commit;
end!

create procedure AssegnaTurno(
	in dataOraInizio timestamp,
	in usernameCameriere varchar(10),
	in numTavolo smallint
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into TurnoAssegnato(Turno, Tavolo, Cameriere) 
		values(dataOraInizio, numTavolo, usernameCameriere);
	
	commit;
end!

create procedure AggiungiProdottoBar(
	in nome varchar(20),
	in costoUnitario float,
	in isAlcolico boolean
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into ProdottoNelMenu(Nome, CostoUnitario, IsAlcolico, IsBarMenu)
		values(nome, costoUnitario, isAlcolico, true);
	
	commit;
end!

create procedure AggiungiProdottoPizzeria(
	in nome varchar(20),
	in costoUnitario float
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into ProdottoNelMenu(Nome, CostoUnitario, IsAlcolico, IsBarMenu)
		values(nome, costoUnitario, NULL, false);
	
	commit;
end!

create procedure CreaIngrediente(
	in nome varchar(20),
	in numDisponibilitaScorte int,
	in costoAlKg float
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into Ingrediente(Nome, NumDisponibilitaScorte, CostoAlKg)
		values(nome, numDisponibilitaScorte, costoAlKg);
	
	commit;
end!

create procedure CreaComposizione(
	in nomeProdottoMenu varchar(20),
	in nomeIngrediente varchar(20)
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into ComposizioneDiBase(ProdottoNelMenu, Ingrediente)
		values(nomeProdottoMenu, nomeIngrediente);

	commit;
end!

create procedure TogliDalMenu(
	in nomeProdottoMenu varchar(20)
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	delete from ProdottoNelMenu
	where nome = nomeProdottoMenu;

	commit;
end!

create procedure CreaTavolo(
	in numTavolo smallint,
	in numMaxCommensali smallint
)
begin
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	insert into Tavolo(NumTavolo, NumMaxCommensali, IsOccupato)
		values(numTavolo, numMaxCommensali, false);
	
	commit;
end!

create procedure VisualizzaEntrateMensili()
begin
end!

create procedure VisualizzaEntrateGiornaliere()
begin
end!

create procedure AumentaScorteIngrediente()
begin
end!

create procedure RilasciaScontrinoPerTavolo()
begin
end!

create procedure MarcaScontrinoPagatoETavoloLibero()
begin
end!

delimiter ;
