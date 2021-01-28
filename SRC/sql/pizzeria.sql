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

delimiter !

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
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

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

call RegistraUtente("ste","Stefano","Belli", "Cave", '1999-10-08',"Roma", "XXXXYYYYZZZZTTTT", "ste123", 1);

