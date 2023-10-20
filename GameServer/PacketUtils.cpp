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

void PacketUtils::DebugDrawBox(RemoteClientPtr inClient, const FVector& inStart, const FVector& inEnd, const FVector& inExtent, const float& inDuration)
{
#if DEBUG_DRAW
    if (inClient)
    {
        Protocol::S2C_DebugBox debugPacket;
        debugPacket.mutable_start_location()->CopyFrom(PacketUtils::ToSVector(inStart));
        debugPacket.mutable_end_location()->CopyFrom(PacketUtils::ToSVector(inEnd));
        debugPacket.mutable_extent()->CopyFrom(PacketUtils::ToSVector(inExtent));
        debugPacket.set_duration(inDuration);

        SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, debugPacket);
        inClient->Send(sendBuffer);
    }
#endif // DEBUG_DRAW
}

void PacketUtils::DebugDrawBox(const std::set<RemoteClientPtr>& inClients, const FVector& inStart, const FVector& inEnd, const FVector& inExtent, const float& inDuration)
{
#if DEBUG_DRAW
    
    Protocol::S2C_DebugBox debugPacket;
    debugPacket.mutable_start_location()->CopyFrom(PacketUtils::ToSVector(inStart));
    debugPacket.mutable_end_location()->CopyFrom(PacketUtils::ToSVector(inEnd));
    debugPacket.mutable_extent()->CopyFrom(PacketUtils::ToSVector(inExtent));
    debugPacket.set_duration(inDuration);

    SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, debugPacket);

    for (auto player : inClients)
    {
        if (player)
        {
            player->Send(sendBuffer);
        }
    }

#endif // DEBUG_DRAW
}

void PacketUtils::DebugDrawSphere(RemoteClientPtr inClient, const FVector& inLocation, const float& inRadius, const float& inDuration)
{
#if DEBUG_DRAW
    if (inClient)
    {
        Protocol::S2C_DebugCircle debugPacket;
        debugPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));
        debugPacket.set_radius(inRadius);
        debugPacket.set_duration(inDuration);

        SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, debugPacket);
        inClient->Send(sendBuffer);
    }
#endif // DEBUG_DRAW
}

void PacketUtils::DebugDrawSphere(const std::set<RemoteClientPtr>& inClients, const FVector& inLocation, const float& inRadius, const float& inDuration)
{
#if DEBUG_DRAW

    Protocol::S2C_DebugCircle debugPacket;
    debugPacket.mutable_location()->CopyFrom(PacketUtils::ToSVector(inLocation));
    debugPacket.set_radius(inRadius);
    debugPacket.set_duration(inDuration);

    SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, debugPacket);

    for (auto player : inClients)
    {
        if (player)
        {
            player->Send(sendBuffer);
        }
    }

#endif // DEBUG_DRAW
}
