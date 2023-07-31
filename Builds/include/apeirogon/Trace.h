#pragma once

enum class ETraceType
{
	Trace_None,
	Trace_Box,
	Trace_Capsule,
	Trace_Sphere,
	Trace_Frustum,
};

class APEIROGON_API Trace
{
public:
	Trace(FVector inStart, FVector inEnd, bool inIsIgnore, ETraceType inType) : mStart(inStart), mEnd(inEnd), mIsIgnore(inIsIgnore), mTraceType(inType) {}
	virtual ~Trace() {}

	Trace(const Trace&) = delete;
	Trace(Trace&&) noexcept = delete;

	Trace& operator=(const Trace&) = delete;
	Trace& operator=(Trace&&) noexcept = delete;

public:
	bool HitCollision(Collision& inCollision);

public:
	const ETraceType&		GetTraceType() const; 

public:
	virtual bool BoxCollisionTrace(BoxCollisionComponent& inBoxCollisionComponent)				abstract;
	virtual bool CapsuleCollisionTrace(CapsuleCollisionComponent& inCapsuleCollisionComponent)	abstract;
	virtual bool SphereCollisionTrace(SphereCollisionComponent& inSphereCollisionComponent)		abstract;

protected:
	FVector		mStart;
	FVector		mEnd;
	bool		mIsIgnore;
	ETraceType	mTraceType;
};

class APEIROGON_API BoxTrace : public Trace
{
public:
	BoxTrace(FVector inStart, FVector inEnd, bool inIsIgnore, FVector inBoxExtent, FRotator inOrientation);
	virtual ~BoxTrace() {}

	BoxTrace(const BoxTrace&) = delete;
	BoxTrace(BoxTrace&&) noexcept = delete;

	BoxTrace& operator=(const BoxTrace&) = delete;
	BoxTrace& operator=(BoxTrace&&) noexcept = delete;

public:
	const float				GetBoxLength();

public:
	virtual bool BoxCollisionTrace(BoxCollisionComponent& inBoxCollisionComponent)				override;
	virtual bool CapsuleCollisionTrace(CapsuleCollisionComponent& inCapsuleCollisionComponent)	override;
	virtual bool SphereCollisionTrace(SphereCollisionComponent& inSphereCollisionComponent)		override;

private:
	BoxCollision mBoxCollision;
};

class APEIROGON_API CapsuleTrace : public Trace
{
public:
	CapsuleTrace(FVector inStart, FVector inEnd, bool inIsIgnore, const float inRadius, const float inHeight);
	virtual ~CapsuleTrace();

	CapsuleTrace(const CapsuleTrace&) = delete;
	CapsuleTrace(CapsuleTrace&&) noexcept = delete;

	CapsuleTrace& operator=(const CapsuleTrace&) = delete;
	CapsuleTrace& operator=(CapsuleTrace&&) noexcept = delete;

public:
	virtual bool BoxCollisionTrace(BoxCollisionComponent& inBoxCollisionComponent)				override;
	virtual bool CapsuleCollisionTrace(CapsuleCollisionComponent& inCapsuleCollisionComponent)	override;
	virtual bool SphereCollisionTrace(SphereCollisionComponent& inSphereCollisionComponent)		override;

private:
	CapsuleCollision mCapsuleCollision;
};

class APEIROGON_API SphereTrace : public Trace
{
public:
	SphereTrace(FVector inStart, FVector inEnd, bool inIsIgnore, float inRadius);
	virtual ~SphereTrace();

	SphereTrace(const SphereTrace&) = delete;
	SphereTrace(SphereTrace&&) noexcept = delete;

	SphereTrace& operator=(const SphereTrace&) = delete;
	SphereTrace& operator=(SphereTrace&&) noexcept = delete;

public:
	virtual bool BoxCollisionTrace(BoxCollisionComponent& inBoxCollisionComponent)				override;
	virtual bool CapsuleCollisionTrace(CapsuleCollisionComponent& inCapsuleCollisionComponent)	override;
	virtual bool SphereCollisionTrace(SphereCollisionComponent& inSphereCollisionComponent)		override;

private:
	SphereCollision mSphereCollision;
};

class APEIROGON_API FrustumTrace : public Trace
{
public:
	FrustumTrace(FVector inStart, FVector inEnd, bool inIsIgnore, float inNearPlane, float inFarPlane, float inHeight);
	virtual ~FrustumTrace();

	FrustumTrace(const FrustumTrace&) = delete;
	FrustumTrace(FrustumTrace&&) noexcept = delete;

	FrustumTrace& operator=(const FrustumTrace&) = delete;
	FrustumTrace& operator=(FrustumTrace&&) noexcept = delete;

public:
	virtual bool BoxCollisionTrace(BoxCollisionComponent& inBoxCollisionComponent)				override;
	virtual bool CapsuleCollisionTrace(CapsuleCollisionComponent& inCapsuleCollisionComponent)	override;
	virtual bool SphereCollisionTrace(SphereCollisionComponent& inSphereCollisionComponent)		override;

private:
	FrustumCollision mFrustumCollision;
};