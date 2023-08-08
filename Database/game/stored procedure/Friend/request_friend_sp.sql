SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	request_friend_sp(ģ�� ��û)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @friend_name			: ģ�� �̸�
-- @action				: �׼� (���� = 0, ��û = 1, ���� = 2)
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS request_friend_sp;
GO

CREATE PROCEDURE request_friend_sp
	@character_id	INT,
	@friend_name	NVARCHAR(8),
	@action			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;
		
		--ģ�� ���̵� �˻�
		DECLARE @friend_id INT
		SELECT @friend_id=id FROM character_tb WHERE name=@friend_name

		IF @friend_id IS NULL
			BEGIN
				ROLLBACK TRANSACTION;
				return 4001
			END

		--�̹� ģ������ Ȯ��
		IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id AND is_friend=1)
			BEGIN
				ROLLBACK TRANSACTION;
				return 4003
			END

		--action�� ��û�� ���
		IF @action=1
			BEGIN
				--������ �̹� ��û�� �ɾ�ξ��� ���
				IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=2)
					BEGIN
						--�ڽ����״� ģ��1 ��û0���� �����ؾ���
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@character_id AND friend_character_id=@friend_id

						--������� ������ �������ϱ� Insert�ؾ���
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
				--������ �ڽ��� �̹� ��û�� �Ǿ��ִ��� Ȯ��
				ELSE IF NOT EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=1)
					BEGIN
						--ģ���ʿ��ٰ� ������ �ڽ����״� ��û�� Insert��
						INSERT friend_tb (user_character_id, friend_character_id, is_friend, is_request, is_block) VALUES (@friend_id, @character_id, 0, 2, 0)
						INSERT friend_tb (user_character_id, friend_character_id, is_friend, is_request, is_block) VALUES (@character_id, @friend_id, 0, 1, 0)
					END
				ELSE
					BEGIN
						ROLLBACK TRANSACTION;
						return 4005
					END
			END
		ELSE IF @action=2
			BEGIN
				--�ڽ����� ��û�� ���� �־����� Ȯ��
				IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=2)
					BEGIN
						--�ڽ����״� ģ��1 ��û0���� �����ؾ���
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@character_id AND friend_character_id=@friend_id

						--������� ������ �������ϱ� Insert�ؾ���
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
				ELSE
					BEGIN
						ROLLBACK TRANSACTION;
						return 4006
					END
			END
		ELSE
			BEGIN
				--��ȿ���� ���� �׼�
				ROLLBACK TRANSACTION;
				return 4009
			END
		
		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

-- ==================================== --
--										--
--				  TEST					--
--										--
-- ==================================== --
USE game_database;

DECLARE @character_id	INT			= 2
DECLARE @friend_name	NVARCHAR(8) = '���'
DECLARE @ret			INT

EXEC @ret=request_friend_sp @character_id, @friend_name, 2
SELECT * FROM friend_tb
SELECT @ret AS '���'

GO