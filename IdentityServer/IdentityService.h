#pragma once
class IdentityService : public Service
{
public:
	IdentityService();
	virtual ~IdentityService();

	IdentityService(const IdentityService& service) = delete;
	IdentityService& operator=(const IdentityService& service) = delete;

public:
	virtual void			OnServiceOpen();
	virtual void			OnServiceClose();

protected:
	virtual bool		SettingService() override;

private:

};

