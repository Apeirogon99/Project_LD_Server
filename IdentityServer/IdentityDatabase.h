#pragma once

enum class EDatabase
{
	Account,
	Game,
};

class IdentityDatabase : public DatabaseManager
{
public:
	IdentityDatabase(const size_t poolSize);
	virtual ~IdentityDatabase();

	IdentityDatabase(const IdentityDatabase&) = delete;
	IdentityDatabase(IdentityDatabase&&) = delete;
	IdentityDatabase& operator=(const IdentityDatabase&) = delete;
	IdentityDatabase& operator=(IdentityDatabase&&) = delete;

protected:
	virtual void InitializeDatabase() override;

private:

};

