#pragma once
class IdentityTask : public TaskManager
{
public:
	IdentityTask();
	virtual ~IdentityTask();

private:
	IdentityTask(const IdentityTask&) = delete;
	IdentityTask(IdentityTask&&) = delete;

	IdentityTask& operator=(const IdentityTask&) = delete;
	IdentityTask& operator=(IdentityTask&&) = delete;

public:
	virtual void Init() override;

public:
	IdentityTaskPtr	GetTaskPtr();
	LoginWorldPtr	GetWorld();
};

