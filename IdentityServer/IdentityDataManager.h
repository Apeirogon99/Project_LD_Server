#pragma once

class IdentityDataManager : public DataManager
{
public:
	IdentityDataManager(const int32 inMaxDatas);
	virtual ~IdentityDataManager();

private:
	IdentityDataManager(const IdentityDataManager&) = delete;
	IdentityDataManager(IdentityDataManager&&) = delete;
	IdentityDataManager& operator=(const IdentityDataManager&) = delete;
	IdentityDataManager& operator=(IdentityDataManager&&) = delete;

public:
	virtual bool InitDatas() override;

private:

};

