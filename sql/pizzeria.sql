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
	unique(Nome, 
		Cognome, 
		ComuneResidenza, 
		DataNascita, 
		ComuneNascita, 
		CF, 
		IsManager)
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
	)
);

create table Tavolo(
	NumTavolo smallint primary key,
	NumMaxPersone smallint not null,
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
	NumOrdinazionePerTavolo smallint,
	DataOraRichiesta timestamp not null,
	Costo float not null,
	DataOraCompletamento timestamp,
	primary key(TavoloOccupato, NumOrdinazionePerTavolo),
	check(
		Costo > 0
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
	NumDisponibilitaScorte smallint not null,
	CostoAlKg float not null
	check(
		CostoAlKg > 0 and NumDisponibilitaScorte >= 0
	)
);

create table ComposizioneDiBase(
	ProdottoNelMenu varchar(20) references ProdottoNelMenu(Nome),
	Ingrediente varchar(20) references Ingrediente(Nome),
	primary key (ProdottoNelMenu, Ingrediente)
);

create table SceltaDelCliente(
	TavoloOccupato timestamp,
	NumOrdinazionePerTavolo smallint,
	NumSceltaPerOrdinazione smallint,
	DataOraCompletamento timestamp,
	DataOraEspletata timestamp,
	ProdottoNelMenu varchar(20) references ProdottoNelMenu(Nome),
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

delimiter !

create procedure CreateUser_Internal(
	in username varchar(10), 
	in password varchar(30))
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	set @sql := concat("create user ", username, "@'%' identified by ", password);
	prepare stmt from @sql;
	execute stmt;
	deallocate prepare stmt;
end!

create procedure GivePrivOnResToUser_Internal(
	in username varchar(10),
	in privlst varchar(100),
	in singleres varchar(50),
	in grantOpt boolean)
begin
	declare exit handler for sqlexception
	begin
		resignal;
	end;

	set @sql := concat("grant ", privlst, " on ", singleres, " to ", username);
	if grantOpt = true then
		set @sql := concat(@sql, " with grant option");
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
		resignal; -- raise again the sql exception to the caller
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, true);
	
	call CreateUser_Internal(username, password);
	call GivePrivOnResToUser_Internal(concat(username,"@'%'"), "all privileges", "pizzeriadb.*", true);
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
		resignal; -- raise again the sql exception to the caller
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_LavoratoreCucina_Internal(username, false);

	call CreateUser_Internal(username, password);
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
		resignal; -- raise again the sql exception to the caller
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_LavoratoreCucina_Internal(username, true);

	call CreateUser_Internal(username, password);
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
		resignal; -- raise again the sql exception to the caller
	end;
	
	call Insertion_Lavoratore_Internal(nome, cognome, comuneResidenza, 
		dataNascita, comuneNascita, cf, username, false);

	call Insertion_Cameriere_Internal(username);

	call CreateUser_Internal(username, password);
end!

-- Cameriere

create procedure SituazioneTavoliAssegnati()
begin
end!

create procedure PrendiComanda()
begin
end!

create procedure ConsegnaOrdinazione()
begin
end!

-- Pizzaiolo e barman

create procedure SituazioneOrdiniAncoraInAttesa()
begin
end!

create procedure PrendiInCaricoOrdine()
begin
end!

create procedure EspletaOrdine()
begin
end!

-- Manager

create procedure AssegnaNuovoTavoloAlCliente()
begin
end!

create procedure RilasciaScontrinoPerTavolo()
begin
end!

create procedure MarcaScontrinoPagatoETavoloLibero()
begin
end!

create procedure AssegnaTurno()
begin
end!

create procedure VisualizzaEntrate()
begin
end!

create procedure AumentaScorteIngrediente()
begin
end!

create procedure DiminuisciScorteIngrediente()
begin
end!

create procedure CreaProdotto()
begin
end!

create procedure OttieniMenu()
begin
end!

create procedure AggiungiAlMenu()
begin
end!

create procedure TogliDalMenu()
begin
end!

delimiter ;
