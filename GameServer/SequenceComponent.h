#pragma once

template<typename T>
class SequenceComponent
{
public:
	SequenceComponent() : mPlay(false), mSequenceID(0), mPlayTime(0), mMaxPlayers(0)
	{

	}

	~SequenceComponent()
	{

	}

	SequenceComponent(const SequenceComponent&) = delete;
	SequenceComponent(SequenceComponent&&) noexcept = delete;

	SequenceComponent& operator=(const SequenceComponent&) = delete;
	SequenceComponent& operator=(SequenceComponent&&) noexcept = delete;

public:
	void Init(std::weak_ptr<T> inOwner, std::function<void(T&)> inEndCallBack)
	{
		mOwner			= inOwner;
		mEndCallBack	= inEndCallBack;
	}

	void Update(const int64& inDeletaTime)
	{
		if (false == IsPlay())
		{
			return;
		}

		mPlayTime -= inDeletaTime;
		if (mPlayTime >= 0)
		{
			return;
		}

		this->End();
	}

	void Play()
	{
		if (0 == GetSequenceID())
		{
			return;
		}

		if (0 == GetPlayTime())
		{
			return;
		}

		if (true == IsPlay())
		{
			return;
		}

		this->mPlay = true;
	}

	bool Skip(const int64& inSkipRemoteID)
	{
		mSkipPlayers.insert(inSkipRemoteID);

		int32 size = static_cast<int32>(mSkipPlayers.size());
		if (size == mMaxPlayers)
		{
			this->End();
			return true;
		}

		return false;
	}

	void End()
	{
		std::shared_ptr<T> owner = mOwner.lock();
		if (nullptr == owner)
		{
			return;
		}

		if (mEndCallBack)
		{
			mEndCallBack(*owner);
		}

		mPlay			= false;
		mPlayTime		= 0;
		mSequenceID		= 0;
		mSkipPlayers.clear();
	}

public:
	void SetSequenceID(const int32& inSequenceID)
	{
		mSequenceID = inSequenceID;
	}

	void SetPlayTime(const int64& inPlayTime)
	{
		mPlayTime = inPlayTime;
	}

	void SetMaxPlayers(const int32& inMaxPlayers)
	{
		mMaxPlayers = inMaxPlayers;
	}


public:
	bool IsPlay() const
	{
		return mPlay;
	}

	int32 GetSequenceID()
	{
		return mSequenceID;
	}

	int64 GetPlayTime()
	{
		return mPlayTime;
	}

	int32 GetSkipPlayers()
	{
		return static_cast<int32>(mSkipPlayers.size());
	}

private:
	bool	mPlay;
	int32	mSequenceID;
	int64	mPlayTime;

	int32			mMaxPlayers;
	std::set<int64> mSkipPlayers;

	std::weak_ptr<T> mOwner;
	std::function<void(T&)> mEndCallBack;
};

