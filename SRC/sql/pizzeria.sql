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

	set transaction isolation level read committed;

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

	set transaction isolation level read committed;

	start transaction;

	update
		UtenteLavoratore
	set
		Passwd = SHA1(pwd)
	where
		Username = usern;
	
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

call RegistraUtente("ste","Stefano","Belli", "Cave", '1999-10-08',"Roma", "XXXXYYYYZZZZTTTT", "ste123", 1);

