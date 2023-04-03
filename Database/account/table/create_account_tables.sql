USE account_database

DROP TABLE IF EXISTS confirm_email_tb;
DROP TABLE IF EXISTS profile_tb;
DROP TABLE IF EXISTS logged_in;
DROP TABLE IF EXISTS auth_password_tb
DROP TABLE IF EXISTS user_tb;
GO

CREATE TABLE user_tb
(
	global_id		INT IDENTITY(1, 1) NOT NULL PRIMARY KEY,
	name			NVARCHAR(24) NOT NULL UNIQUE,
	enable			INT DEFAULT 0 CHECK(enable = 1 OR enable = 0) NOT NULL
)
GO

CREATE TABLE auth_password_tb
(
	global_id		INT NOT NULL FOREIGN KEY REFERENCES user_tb(global_id) ON DELETE CASCADE,
	password		NVARCHAR(24) NOT NULL,
	salt			VARCHAR(126) NOT NULL,
	update_date		DATETIME DEFAULT SYSDATETIME() NOT NULL
)
GO

CREATE TABLE confirm_email_tb
(
	global_id			INT NOT NULL FOREIGN KEY REFERENCES user_tb(global_id) ON DELETE CASCADE,
	local				NVARCHAR(64) NOT NULL,
	domain				NVARCHAR(255) NOT NULL,
	verify_code			INT DEFAULT NULL NULL,
	verify				INT DEFAULT 0 CHECK (verify = 1 OR verify = 0),
	expired_date		DATETIME DEFAULT SYSDATETIME() NOT NULL,
)
GO

CREATE TABLE profile_tb
(
	global_id		INT NOT NULL FOREIGN KEY REFERENCES user_tb(global_id) ON DELETE CASCADE,
	create_date		DATETIME DEFAULT SYSDATETIME() NOT NULL,
	update_date		DATETIME DEFAULT SYSDATETIME() NOT NULL,
)
GO

CREATE TABLE logged_in
(
	global_id		INT NOT NULL FOREIGN KEY REFERENCES user_tb(global_id),
	remote_addr		VARCHAR(255) NOT NULL,
	result			INT NOT NULL,
	login_date		DATETIME DEFAULT SYSDATETIME() NOT NULL,
)
Go

SELECT * FROM user_tb
SELECT * FROM confirm_email_tb
SELECT * FROM profile_tb
SELECT * FROM logged_in
GO