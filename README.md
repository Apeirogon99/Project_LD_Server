  <!--Header-->
  <h1> Dark Crown : Return of the Lich </h1>
  <h2> 논 타겟팅 액션 MMORPG </h2>
</div>

<div>
  
  ### 📋구현 요약 : </br>
  #### System
  - 전투 시스템
    - [몬스터 인공지능](#몬스터-인공지능) : FSM을 이용한 단순한 인공지능 적용
    - [충돌 감지](#충돌-감지) : KD-Tree를 이용하여 충돌 감지
      - [충돌 테스트](https://github.com/Apeirogon99/CollisionDetection) : Array, Quad-Tree, KD-Tree 비교 및 분석
    - [공격 방식](#공격-방식) : Shot -> Target -> Over 순서 적용
  - [이동 동기화](#이동-동기화) : 데드레커닝을 이용한 동기화
    - [데드레커닝 개선](https://github.com/Apeirogon99/MovementSync) : 데드레커닝 방향전환으로 인한 위치 오차를 줄이기 위한 동적 동기화 적용
  - [가시거리](#가시거리) : 트래픽을 줄이기 위한 가시거리 제작 및 활용
    - [가시거리 개선](https://github.com/Apeirogon99/AreaOfInterest) : 플레이어 시야기준 가시영역을 나누어 정보 및 주기를 나눠 트래픽 감소 적용
  #### Tool
  - [위젯 관리](https://github.com/Apeirogon99/Project_LD/tree/master/Source/Project_LD/Public/Widget) : 언리얼 엔진의 HUD를 이용하여 위젯 관리
  #### Content
  - 전투
    - [스킬](#스킬) : 설치형, 차징, 대쉬, 패링 스킬 구현
    - [패턴](#패턴) : 3페이즈 보스의 스킬 및 다크 나이트 애니메이션 구현
    - [버프/디버프](#버프-디버프) : 캐릭터의 능력치 버프/디버프와 시야를 가리는 블라인드 구현
  - SNS
    - [전체 채팅](#채팅) : 주변에 있다면 채팅을 볼 수 있는 전체 채팅
    - [파티](#파티) : 파티를 추가, 추방, 위임하고 던전을 같이 참여 가능
    - [친구](#친구) : 친구를 추가하여 온라인 상태, 오프라인 상태 확인 가능
  - 캐릭터
    - [캐릭터 커스텀](#캐릭터-커스텀) : 캐릭터의 외형을 변경하고 저장 및 로드할 수 있는 커스터마이징 시스템
  - 아이템
    - [루팅](#루팅) : 등급별 아이템이 존재하고 줍고 버릴 수 있는 루팅 시스템
    - [장비](#장비) : 장비에 따라 캐릭터의 스탯이 증가하고 시각적 외형 변경
    - [인벤토리](#인벤토리) : 그리드 형식의 인벤토리 처리
  - 계정
    - [회원가입 및 인증](#회원가입-및-인증) : 회원가입하고 이메일로 인증해야 최종적으로 가입되는 시스템  
</div>

<div align="center"> <h1> System </h1> </div>
<div align="center"> <h2> 전투 시스템 </h2> </div>

# 몬스터 인공지능
### 설계 목표
보스, 일반 몬스터에 사용할 단순한 인공지능 필요했습니다. </br>

### FSM을 이용한 단순한 인공지능 처리 
많은 상태가 필요하지 않기에 FSM을 이용하여 인공지능 처리할 수 있도록 하였습니다. </br>
몬스터의 생성부터 죽음까지 거치는 행동 상태와 전환 조건을 정의한 다이어그램입니다. </br>

<img width="1080" height="494" alt="image" src="https://github.com/user-attachments/assets/db4d635c-64fd-442b-a768-980b097a91bd" />

### 몬스터 초기화
```
void EnemySlime::OnInitialization()
{
	//Tick 사용
	SetTick(true, SYSTEM_TICK);

	//몬스터 타입 : 일반 몬스터
	this->SetEnemyType(EnemyType::Nomal);

	//몬스터 상태 : 쉬고 있는 상태
	this->mStateManager.SetEnemy(GetEnemyCharacterRef());
	this->mStateManager.SetState(EStateType::State_Idle);

	//몬스터 스탯 동기화 설정
	this->mStatsComponent.SetSyncTime(GAME_TICK);

	//몬스터 콜리전 추가
	BoxCollisionComponent* collision = this->GetBoxCollisionComponent();
	collision->SetOwner(this->GetActorRef());
	collision->SetBoxCollision(FVector(62.0f, 62.0f, 96.0f));

	//몬스터 위치 동기화 설정
	this->mMovementComponent.InitMovement(this->GetLocation(), GAME_TICK, world->GetWorldTime());

	Location initLocation = this->GetLocation();
	this->mMovementComponent.SetNewDestination(this->GetActorPtr(), initLocation, initLocation, world->GetWorldTime(), 62.0f);

	//몬스터 공격 정보 입력
	AttackInfos infos;
	infos.push_back(AttackInfo(0, 260, 1000, FVector(110.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
}
```

### 상태별 처리 요약
모든것을 말하는 것 보다는 몇가지만 소개하겠습니다.

#### Round </br>
Enter에 목표 위치가 정해지고 (스폰 위치에 따라서) </br>
Update에 목표와 가까워지면 다음 패턴으로 넘어 가도록 되어있습니다. </br>

<img width="1080" height="119" alt="image" src="https://github.com/user-attachments/assets/ca72697e-aa09-4d01-9f80-f44f43386c81" />

<img width="766" height="22" alt="image" src="https://github.com/user-attachments/assets/28e5ba9c-92bb-41cf-96ff-e72a060dbef1" />


#### Attack
공격은 이전 공격 방식에서 설명한 Shot -> Target -> Over 순서에 따라 이루어 집니다 </br>
또한 자신의 Attack Type에 따라 다르게 호출하도록 하였습니다 </br>

<img width="464" height="29" alt="image" src="https://github.com/user-attachments/assets/99660d28-11d5-4a50-9bdf-5ed118f01038" /> </br>

슬라임, 스켈레톤 일반, 스켈레톤 전사의 경우 Melee </br>
<img width="772" height="45" alt="image" src="https://github.com/user-attachments/assets/e5d3de0d-096a-48f2-a20f-4e841d30840e" /> </br>

스켈레톤 궁수의 경우 Ranged </br>
<img width="766" height="41" alt="image" src="https://github.com/user-attachments/assets/619a3784-f5f4-40ff-b56b-06665a2f1dc0" /> </br>

보스 몬스터의 경우 Patten </br>
<img width="395" height="58" alt="image" src="https://github.com/user-attachments/assets/0901add1-e2ce-4e62-92f0-eb54e8cfe760" /> </br>

이렇게 구분하여 공격할때 Enemy마다 다른 공격을 할 수 있도록 하였습니다.  </br>

### Hit
외부에서 대미지가 들어올 경우  </br>
현재 방어력과 체력을 계산하여 대미지를 입히고 0이면 Death를 아니라면 Hit상태로 변환하였습니다.  </br>

<img width="568" height="28" alt="image" src="https://github.com/user-attachments/assets/071aceb2-cb95-4da8-abb3-a2af94f5afb4" /> </br>

<img width="833" height="77" alt="image" src="https://github.com/user-attachments/assets/8f2a718d-cc58-4e1a-818b-e7d22dc34761" /> </br>

<img width="433" height="27" alt="image" src="https://github.com/user-attachments/assets/71b9d8c8-9189-4278-b4a5-59af7a2d15a7" /> </br>
<img width="416" height="24" alt="image" src="https://github.com/user-attachments/assets/ec42ba1f-0486-45a3-8ef5-f27275ea1f36" /> </br>

# 충돌 감지
### 설계 목표
근접 공격, 원거리 공격, 스킬 등 모든 공격을 논타겟으로 제작하고자 하였습니다. </br>

### KD-Tree이용하여 충돌 처리
모든 액터들을 가시거리로 처리하고 있었으며 기반이 다져진 상태였습니다. </br>
고민 하던 중 특정 지점 근처의 객체를 빠르게 찾거나 일정 범위 내 객체를 검색 속도에 이점이 있는 KD-Tree를 </br>
선택하였으며, 이는 지금까지 만들어둔 스킬, 공격등 매우 어울린다고 생각하였습니다. </br>

### [충돌 테스트](https://github.com/Apeirogon99/CollisionDetection) : Array, Quad-Tree, KD-Tree 비교 및 분석
KD-Tree의 단점인 동적 객체의 재배치 비용과 데이터 규모에 대하여 KD-Tree가 정말 맞는 판단이었는지 궁금하여 비교 및 분석 하였습니다 </br>

<img width="1919" height="513" alt="image" src="https://github.com/user-attachments/assets/34a859b3-5e9b-4048-aa0b-8e720fcfdd04" /> </br>

### 충돌 검사 동작 (스켈레톤의 일반 공격)

우선 지속적으로 KD-Tree는 재빌드 되어지고 있는 상태 입니다. </br>

충돌 검사를 위해 정보(콜리전 종류, 타입, 검사 개수)를 넣어야 합니다
콜리전 종류 : 종류는 다음과 같이 선, 원, 박스로 구분되어 있습니다 </br>
타입 : ActorType을 통해 Type별로 검사도 가능합니다 </br>
검사 개수 : MaxResult를 통해 검사하고 싶은 개수를 알 수 있습니다  </br>

<img width="580" height="164" alt="image" src="https://github.com/user-attachments/assets/65003941-fc97-448a-934f-cda6ef5d1b3f" />

#### Radius Search (범위 검색)
처음 부터 자세한 검사(사각형 - 주황색)를 하면 시간이 그만큼 오래 걸린다는 것을 알게 되었습니다  </br>
그래서 그 전에 범위 검색(사각형 외접원 - 파란색)을 통해 시간을 줄일 수 있도록 하였습니다  </br>

<img width="424" height="274" alt="image" src="https://github.com/user-attachments/assets/8121ceaf-132c-4c60-8947-72ce6e9a09c2" /> </br>

우선 파란색인 외접원을 구해주고 검색을 시작합니다. </br>
<img width="845" height="131" alt="image" src="https://github.com/user-attachments/assets/b78f7dcc-2368-4e37-8b47-51e6cc0e13ee" />
<img width="694" height="95" alt="image" src="https://github.com/user-attachments/assets/472f999e-ed09-4bb4-ba12-bf326e2f4e92" />
<img width="752" height="55" alt="image" src="https://github.com/user-attachments/assets/8207e629-e420-4721-8695-07c9f3d8615c" />

그리고 다음과 같이 Actor들의 Collision을 통해 Radius값과 Location값을 받은 이후  </br>
내가 찾는 위치 안에 있는지 그리고 Type은 맞는지 검색하여 GameObjectID값을 넣습니다.  </br>
<img width="623" height="110" alt="image" src="https://github.com/user-attachments/assets/0af66135-7dcb-436f-833d-da2563e93549" />

그리고 다음 노드를 찾기 위해  Depth를 기준으로 처리합니다 (사진은 (X == Depth 0)만 기준) </br>
이렇게 계속 순회를 하면 범위안에 있는 GameObjectID를 얻을 수 있습니다 </br>
<img width="963" height="255" alt="image" src="https://github.com/user-attachments/assets/94d7b986-6aaa-44a6-9c38-433daf2ccc83" />

#### Collision Trace(자세한 검색)
수학을 잘 모르지만 어떻게든 3D간 충돌을 구현하여야 했고 </br>
많은 자료를 보며 우여곡절 끝에 OBB를 제작하였습니다 </br>

다른 충돌들도 만들고 싶었지만 한계로 인하여 </br>
원 vs 원 , 원 vs 박스, 박스 vs 박스, 원 vs 선, 박스 vs 선 정도가 시간상 한계였습니다... </br>

자세한 검색은 위에서도 말했지만 OBB와 같은 처리는 객체의 회전을 고려하기 때문에 더 많은 계산이 필요합니다 </br>
그렇기에 범위 탐색을 먼저 진행하여 얻은 결과물을 토대로 좀 더 자세하게 검색하는 것이 목적입니다 </br>

계속해서 스켈레톤의 일반 공격에 대하여 진행하겠습니다 </br>
처리는 범위 탐색(Radius Search)에서 얻은 결과물을 토대로 좀더 자세하게 검색하는 것이 목적입니다 </br>
우선 actor가 있는지 확인 합니다 </br>

<img width="537" height="129" alt="image" src="https://github.com/user-attachments/assets/af1d0605-feac-45e8-93fa-371520502dae" />

그리고 해당 엑터는 어떤 콜리전인지는 알 수 없기 때문에 콜리전에 대한 컴포넌트를 가져옵니다 </br>
<img width="664" height="114" alt="image" src="https://github.com/user-attachments/assets/a8fcbc8b-98c7-4017-8515-307a3f79b23a" />

그리고 스켈레톤의 일반 공격은 Box이므로 BoxTrace와 검사할 엑터의 콜리전을 넣습니다 </br>
<img width="503" height="101" alt="image" src="https://github.com/user-attachments/assets/fbc36411-bad7-466f-a4fd-ca5143ac8d87" />

Collision Component에 Type에 따라서 처리가 되어집니다 </br>
<img width="733" height="293" alt="image" src="https://github.com/user-attachments/assets/26fd776c-df7a-46a0-aaf8-71338df7caad" />

만약에 맞은것이 확인 되었다면 값을 넣어주고 찾고 싶은 개수와 같다면 중지합니다 </br>
그렇지 않다면 범위탐색에서 찾은 개수만큼 반복하여 종료하게 됩니다 </br>
<img width="462" height="127" alt="image" src="https://github.com/user-attachments/assets/33f3a443-0128-4193-9fb9-6a01c17e75bf" />

마지막으로 충돌된 액터의 개수를 리턴함으로써 충돌한 엑터가 있는지 확인이 가능합니다 </br>
<img width="300" height="38" alt="image" src="https://github.com/user-attachments/assets/02aca9c5-ec5d-49fd-9b85-2dfd3132b208" />


# 공격 방식
### 설계 목표
근접 공격, 연속 공격, 원 거리, 패턴 등 다양한 공격을 처리하기 위한 방식이 필요하였습니다. </br>

### 공격 단계를 나누는 방식
다양한 공격을 처리하기 위해서는 콘텐츠 간의 분리와 확장성을 고민했습니다. </br>
공격이 다양하고 모두 다르게 보이지만 크게 보았을 때 일정한 패턴이 보였습니다. </br>
Shot -> Target -> Over의 단계마다 역할이 존재하며 순서대로 동작되어 처리하며 확장성을 높일 수 있었습니다. </br>

<img width="1914" height="511" alt="image" src="https://github.com/user-attachments/assets/eb349799-bc13-47e0-bf0a-ce93b92e402a" />

### 슬라임 몬스터 공격 (Shot -> Targeting -> Over)

몬스터를 초기화 하는 시점에 공격 정보를 넣어줍니다. </br>
```
//몬스터 공격 정보 입력
	AttackInfos infos;
	infos.push_back(AttackInfo(0, 260, 1000, FVector(110.0f, 100.0f, 100.0f)));
	this->mAutoAttackComponent.InitAutoAttack(EAutoAttackType::Attack_Melee, infos);
```

FSM Attack 상태가 된다면 정해진 시간에 따라 순서대로 호출되어 집니다 </br>
```
void EnemySlime::OnAutoAttackShot(ActorPtr inVictim)
{
	{
    	//공격 한다는 패킷을 보냄
		Protocol::S2C_EnemyAutoAttack autoAttackPacket;
		autoAttackPacket.set_object_id(this->GetGameObjectID());
		autoAttackPacket.set_timestamp(world->GetWorldTime());

		SendBufferPtr sendBuffer = GameServerPacketHandler::MakeSendBuffer(nullptr, autoAttackPacket);
		this->BrodcastPlayerViewers(sendBuffer);
	}
}
```

공격이라는 엑터를 위치와 회전값을 넣어 스폰하고 정보를 입력합니다 </br>
```
void EnemySlime::OnAutoAttackTargeting(const float inDamage, const FVector inRange)
{

	//공격할 엑터 스폰
	ActorPtr attack = world->SpawnActor<EnemyMeleeAttack>(this->GetActorRef(), this->GetLocation(), this->GetRotation(), FVector(1.0f, 1.0f, 1.0f));
	if (nullptr == attack)
	{
		return;
	}

	//공격에 대한 정보 입력
	mMeleeAttack = std::static_pointer_cast<EnemyMeleeAttack>(attack);
	mMeleeAttack->SetEnemyAttackType(EEnemyAttackType::Enemy_Attack_Nomal_Melee);
	mMeleeAttack->SetTargetActorType(EActorType::Player);
	mMeleeAttack->SetAttackExtent(inRange);
	mMeleeAttack->SetDamage(inDamage);
	mMeleeAttack->SetParryinglTime(worldTime, worldTime + 150);

	AttackInfo info = mAutoAttackComponent.GetAttackInfo(0);
	mMeleeAttack->PushTask(worldTime + 200, &EnemyMeleeAttack::CheackCollision);
	mMeleeAttack->PushTask(worldTime + 250, &EnemyMeleeAttack::PushReserveDestroy);

}
```

보통 공격이 끝난다면 현재 상태를 체크하고 다음 상태를 이어나갑니다 </br>
```
void EnemySlime::OnAutoAttackOver()
{
	//공격을 초기화함
	mMeleeAttack.reset();
	this->mAutoAttackComponent.OnOverAutoAttack();

	//죽지 않았다면 다음 행동을 지속함
	if (false == this->IsDeath())
	{
		if (this->mStateManager.GetCurrentStateType() != EStateType::State_Stun)
		{
			this->mStateManager.SetState(EStateType::State_Chase);
		}
	}
}
```

<div align="center"> <h2> 이동 동기화 </h2> </div>

### 설계 목표
매 프레임마다 클라이언트에서 이동에 대한 값을 서버에게 보내주어 동기화 하는 방식을 사용했었습니다. </br>
다른 클라이언트에게 위치를 보내면 수정하는 형식은 대역폭과 서버 부하 문제가 발생했고 해결하고자 하였습니다 </br>

### 서버에게 허락 맡고 이동하기
처음 생각한 것은 클라이언트가 서버에게 허락을 맡고 이동을 하는 것이였습니다. </br>
그런데 팀원과의 레이턴시가 약20ms정도 차이가 있었고 이러한 방식은 미세하지만 약간의 답답함을 느꼈습니다. </br>
그래서 다른 방식을 찾아 약간의 답답한 경험을 개선하고자 하였습니다. </br>

### 데드레커닝
다음 방식은 클라이언트에서 먼저 이동하고 예측과 보정을 하는 데드레커닝 방식을 사용하고자 하였습니다. </br>

### [데드레커닝 개선](https://github.com/Apeirogon99/MovementSync) : 데드레커닝 방향전환으로 인한 위치 오차를 줄이기 위한 동적 동기화 적용
언제나 클라이언트가 먼저 이동하고 서버는 늦게 전달을 받게 됩니다 </br>
일반적으로 비슷한 방향으로 이동할 경우에는 움직임이 자연스러웠습니다. </br>
하지만 90도 이상으로 방향을 변경한 경우 오차가 벌어지게 되는 문제가 있습니다. </br>

<div align="center"> <h2> 가시거리 </h2> </div>

### 설계 목표
모든 플레이어에게 브로드캐스트하는 방식은 트래픽 낭비와 불필요한 정보 전달 문제가 있어 해결하고자 하였습니다. </br>

### 가시거리 기반 패킷 전송
플레이어는 자신과 멀리 떨어진 곳에서 일어나는 일들을 굳이 알 필요가 없다고 생각하였습니다. </br>
현실 세계도 마찬가지로 자신과 멀리 있는 상대방은 서로 어떤 행동을 하고 있는지 알 수 없습니다. </br>
그렇기에 가시영역을 만들어 주변에서 일어나는 일들만 전송할 수 있도록하여 트래픽을 줄이고자 하였습니다. </br>

### [가시거리 개선](https://github.com/Apeirogon99/AreaOfInterest) : 플레이어 시야기준 가시영역을 나누어 정보 및 주기를 나눠 트래픽 감소 적용
가시거리의 특성상 플레이어 화면에 없다가 갑작스럽게 나타나게 된다면 </br>
유저는 어색함을 느끼게 되어 화면보다는 좀 더 큰 범위에 영역을 설정합니다. </br>
문제는 자연스러움을 위해 화면에 보이지 않는 플레이어들의 정보를 모두 동기화  </br>
하는 것은 또 다시 불필요한 트래픽이 발생하고 있어 해결하고자 하였습니다.  </br>

------

<div align="center"> <h1> Content </h1> </div>
<div align="center"> <h2> 전투 </h2> </div>

# 스킬
![parrying](https://github.com/user-attachments/assets/d2a0d1e4-94fb-4c80-8300-ebc8bf6e331a)
![charging](https://github.com/user-attachments/assets/885d132f-07b6-437c-ba42-e3b2590f025a)
![slam](https://github.com/user-attachments/assets/d99e45ef-5a92-4799-8a70-880fec0c87eb)
![buff](https://github.com/user-attachments/assets/23160613-489e-424b-81c7-237fb5cac51d)
![dash](https://github.com/user-attachments/assets/3b2a0249-5ca7-4568-8e2c-f09dbcea05c1)

# 패턴

# 버프 디버프
<img src="https://github.com/user-attachments/assets/c94335f4-5bf0-43d9-a82b-4d4f3566eb05" alt="Video Label" width="400" height="300" />

<div align="center"> <h2> SNS </h2> </div>

# 채팅
<a href="https://youtu.be/V_tvPMT1-Mk?si=5YUewK7-nNEPqBQU&t=55">
  <img src="https://github.com/user-attachments/assets/b99b5e6d-994f-41ce-b270-cdd119cd8db5" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 55초부터 시작)</em>

# 파티
<a href="https://youtu.be/V_tvPMT1-Mk?si=Y0_Fd4UF8YmNELK4&t=78">
  <img src="https://github.com/user-attachments/assets/58b00dd4-72b9-49c6-a233-1c34d522975b" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (1분 18초부터 시작)</em>

# 친구
<a href="https://youtu.be/V_tvPMT1-Mk?si=bAHyILk5hUkYY-zg&t=65">
  <img src="https://github.com/user-attachments/assets/2a318941-19cc-44d6-8e9c-79f4616860fe" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (1분 05초부터 시작)</em>


<div align="center"> <h2> 캐릭터 </h2> </div>

# 캐릭터 커스텀
<a href="https://youtu.be/V_tvPMT1-Mk?si=fdfGr-h5dcJqIC_2&t=19">
  <img src="https://github.com/user-attachments/assets/ad7269f8-ba6b-4674-bd5c-d7a3c85a4b02" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 19초부터 시작)</em>

<div align="center"> <h2> 아이템 </h2> </div>

# 루팅
<img src="https://github.com/user-attachments/assets/351a3709-78df-4fbd-b09a-2caab1d6a914" alt="Video Label" width="400" height="300" />

# 장비
<img src="https://github.com/user-attachments/assets/d5d4367a-baa0-4a8c-b9b5-eeacdd3a3c69" alt="Video Label" width="400" height="300" />

# 인벤토리
<img src="https://github.com/user-attachments/assets/6045c1ad-1ad7-4168-a742-3d2a17c62b08" alt="Video Label" width="400" height="300" />

<div align="center"> <h2> 계정 </h2> </div>

# 회원가입 및 인증
<a href="https://youtu.be/V_tvPMT1-Mk?si=QOYz4I2tYBmWb9ss&t=9">
  <img src="https://github.com/user-attachments/assets/8682c1d0-48fc-4bdd-b104-5729ec97e091" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 09초부터 시작)</em>

