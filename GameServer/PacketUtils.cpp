#include "pch.h"

Transform PacketUtils::ToFTransform(const Protocol::STransform& inTransform)
{
    Transform transform;
    transform.SetLocation(PacketUtils::ToFVector(inTransform.location()));
    transform.SetRotation(PacketUtils::ToFRotator(inTransform.rotation()));
    transform.SetScale(PacketUtils::ToFVector(inTransform.scale()));
    return transform;
}

Protocol::STransform PacketUtils::ToSTransform(const Transform& inTransform)
{
    Protocol::STransform transform;
    transform.mutable_location()->CopyFrom(PacketUtils::ToSVector(inTransform.GetLocation()));
    transform.mutable_rotation()->CopyFrom(PacketUtils::ToSRotator(inTransform.GetRotation()));
    transform.mutable_scale()->CopyFrom(PacketUtils::ToSVector(inTransform.GetScale()));
    return transform;
}

FVector PacketUtils::ToFVector(const Protocol::SVector& inVector)
{
    FVector vector;
    vector.SetX(inVector.x());
    vector.SetY(inVector.y());
    vector.SetZ(inVector.z());
    return vector;
}

Protocol::SVector PacketUtils::ToSVector(const FVector& inVector)
{
    Protocol::SVector vector;
    vector.set_x(inVector.GetX());
    vector.set_y(inVector.GetY());
    vector.set_z(inVector.GetZ());
    return vector;
}

FRotator PacketUtils::ToFRotator(const Protocol::SRotator& inRotator)
{
    FRotator rotator;
    rotator.SetPitch(inRotator.pitch());
    rotator.SetYaw(inRotator.yaw());
    rotator.SetRoll(inRotator.roll());
    return rotator;
}

Protocol::SRotator PacketUtils::ToSRotator(const FRotator& inRotator)
{
    Protocol::SRotator rotator;
    rotator.set_pitch(inRotator.GetPitch());
    rotator.set_yaw(inRotator.GetYaw());
    rotator.set_roll(inRotator.GetRoll());
    return rotator;
}
