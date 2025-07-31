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
    - [공격 방식](#공격-방식) : Shot -> Target -> Over 통일된 공격 순서 적용
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

## 이동 동기화

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

## 가시거리

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

### 설계 목표
패링, 차징, 찍기, 버프, 대쉬를 제작 및 키 입력 처리 </br>

### 키 입력
다른 게임들을 보면 키 세팅이라는 것이 존재합니다  </br>
그렇기 때문에 키를 바꿔도 동작이 되어야 한다는 것을 중점으로 설계를 하였습니다. </br>

아이템 아이디, 스킬 아이디는 고유하기 때문에 다음과 같이 아이디값을 key로 주고 value를 BindAction(아이템, 스킬등..)을 넣어 키에 대한 정보를 확인할 수 있도록 하였습니다  </br>
<img width="323" height="23" alt="image" src="https://github.com/user-attachments/assets/56a2d54d-8fe9-4094-ae0a-cd0cf2db36ca" />

그럼 다음 문제는 "어떻게 실행을 하지?" 였습니다. </br>
입력에 대한 정보를 얻고 Key(아이디) 값을 넣어 실행하기 위해서는 이전에 루키스 강의를 보면서 배운 Handler를 사용하면 좋겠다고 생각하였습니다 </br>
<img width="614" height="113" alt="image" src="https://github.com/user-attachments/assets/68a3a9a4-16db-43bb-b647-d906b331e591" />

그리고 이를 배열에 넣은 함수를 호출하여 사용하였습니다
<img width="705" height="98" alt="image" src="https://github.com/user-attachments/assets/d3aa5403-550b-4d6b-b3ef-b668f86e9c25" />

### 패링
공격 또는 카운터 패턴을 파훼하기 위한 패링 스킬 </br>
![parrying](https://github.com/user-attachments/assets/d2a0d1e4-94fb-4c80-8300-ebc8bf6e331a)

#### 의도치 않은 옆, 뒤 공격도 패링하는 문제
계획한 패링은 정밀한 각도와 타이밍을 요구하였습니다. </br>

그래서 적의 공격 방향과 패링의 방향을 비교하였으며 </br>
원하는 패링 각도를 설정하여 패링을 처리하였습니다. </br>

<img width="570" height="316" alt="image" src="https://github.com/user-attachments/assets/6b524b34-3efb-45f4-a839-e07af675c1fc" />
<img width="575" height="159" alt="image" src="https://github.com/user-attachments/assets/a7630cb1-3b15-4e7f-9bde-f853a2647e54" />

### 차징
차징 스킬로 길게 누르면 대미지와 범위가 늘어나게 됩니다. </br>
![charging](https://github.com/user-attachments/assets/885d132f-07b6-437c-ba42-e3b2590f025a)

키를 눌렀을때 서버에 보내게 되고 서버에서는 시간을 측정합니다. </br>
키에서 손을 놓게 된다면 서버는 최대 시간을 생각하여 검기의 대미지와 범위를 늘려줍니다. </br>
<img width="1756" height="512" alt="image" src="https://github.com/user-attachments/assets/28067070-c95f-4d18-a1b7-5825387b9d6f" />

### 찍기
범위에 따라서 좁은 범위에는 스턴이, 큰 범위에는 슬로우가 걸리게 됩니다. </br>
거리가 멀수록 대미지가 약해집니다 </br>
![slam](https://github.com/user-attachments/assets/d99e45ef-5a92-4799-8a70-880fec0c87eb)

<img width="2560" height="952" alt="image" src="https://github.com/user-attachments/assets/8e668759-dc0e-4042-a14b-0013af0d69d7" />

### 버프
플레이어 및 파티에게 마법진 안에 들어가 있으면 이로운 버프를 줍니다. </br>
버프의 아이콘의 경우 들어온 버프에따라 Ex) "공격력 10퍼 증가" 라면 buff id를 보내 버프가 사라질때까지 플레이어 정보에 뜨게 해줍니다. </br>
이는 디버프도 마찬가지입니다. </br>
![buff](https://github.com/user-attachments/assets/23160613-489e-424b-81c7-237fb5cac51d)

버프는 장판 지속이기 때문에 이미 버프를 받았다면 중첩되어 받지 않게 관리했습니다
<img width="2442" height="792" alt="image" src="https://github.com/user-attachments/assets/4e011317-7f3d-49fc-93c5-acca19b7a221" />

### 대쉬
플레이어가 바라보는 방향으로 짧게 돌진합니다. </br>
앞에 장애물이 있다면 장애물 앞까지만 돌진합니다. </br>
![dash](https://github.com/user-attachments/assets/3b2a0249-5ca7-4568-8e2c-f09dbcea05c1)

장애물까지의 거리를 측정하기 위해서는 장애물에 대한 정보가 필요했고 </br>
다음과 같이 플레이어가 바라보는 방향에서 직선의 형태의 선을 그어서 장애물이 존재한다면 그 앞에 멈추도록 하였습니다 </br>
플레이어의 반지름을 구해 주황색처럼 막힌 부분이 아니라 초록색 처럼 처리하였습니다 </br>

<img width="2442" height="712" alt="image" src="https://github.com/user-attachments/assets/91a38619-fea7-44fe-a108-aa79a2d9b7b4" />

<img width="868" height="318" alt="image" src="https://github.com/user-attachments/assets/7ca3b7ce-9b08-484c-94a1-d0c0af199622" />


# 패턴

### 설계 목표
공격 방식을 응용하여 패턴을 적용하고자 하였습니다. </br>
마찬가지로 몬스터 초기화 시점에 패턴에 대한 정보를 넣습니다. </br>
<img width="1826" height="394" alt="image" src="https://github.com/user-attachments/assets/b9bb4f0a-e8a2-42aa-8a1d-f5140b9300a5" />

그리고 Shot에서 랜덤으로 패턴을 실행하게 하여 처리하였습니다. </br>
<img width="1906" height="394" alt="image" src="https://github.com/user-attachments/assets/67b2d62f-66b5-41a6-9be7-5bed2a4932b6" />

다음은 페이즈별 스킬입니다. </br>
위의 스킬들을 대부분 응용한 것이기에 영상만 넣었습니다. </br>

### 페이즈 1
처음은 리치를 상대하게 됩니다. </br>

![phase1-1](https://github.com/user-attachments/assets/5f4428da-f469-4cf3-b3ce-d19d8d09374a)

![phase1-2](https://github.com/user-attachments/assets/661456aa-d441-45a5-aa6e-3dffa632207e)

![phase1-3](https://github.com/user-attachments/assets/0f5f8a04-70ce-47f9-8961-bfe07e705526)

![phase1-4](https://github.com/user-attachments/assets/3f1c1501-539b-4148-abfd-6b08e86b81a9)


### 페이즈 2
리치는 다크나이트를 소환하고 무적상태에 돌입합니다. </br>
다크나이트를 물리치면 페이즈가 넘어갑니다. </br>

![phase2-1](https://github.com/user-attachments/assets/78245cbb-9d2c-451d-a96e-744a585e7729)

![phase2-2](https://github.com/user-attachments/assets/77ad4d41-5028-40ab-b1ed-e97eae8b3884)

![phase2-3](https://github.com/user-attachments/assets/a98caaf1-88ae-48fc-abca-b63dec0ff0b3)

![phase2-4](https://github.com/user-attachments/assets/445524e5-9747-4f26-89a3-bbe68b506e33)

![phase2-5](https://github.com/user-attachments/assets/adccdadd-662a-46cc-a593-23d0421e89b5)

![phase2-6](https://github.com/user-attachments/assets/81ffe7df-71ae-4b50-865b-2a9436cf44d6)

![phase2-7](https://github.com/user-attachments/assets/2cba7196-5dde-4698-9444-d3df1851bfe0)

![phase2-8](https://github.com/user-attachments/assets/5a0d51f3-3dfe-4eb3-ab67-f4cb27f9fd95)

![phase2-9](https://github.com/user-attachments/assets/49816256-8c88-44da-9774-dc112d75ca3b)

![phase2-10](https://github.com/user-attachments/assets/07a82d5f-863f-4175-857f-534a3fe2e858)

### 페이즈 3
마지막 페이즈는 가려진 안개속에서 리치의 심장을 찾아 부수면 클리어됩니다.

![phase3-1](https://github.com/user-attachments/assets/c32c4209-763a-4ce1-a3e6-99e4f13269c0)

![phase3-2](https://github.com/user-attachments/assets/13718400-d42b-4a5f-8b91-4a8b5584e443)

![phase3-3](https://github.com/user-attachments/assets/63a9e437-ece8-4695-9e4f-91687bf24a3b)

![phase3-4](https://github.com/user-attachments/assets/66b2f495-8d68-4d2e-b5d0-3e420448858d)

### 다크 나이트 애니메이션 동기화
다크 나이트 애니메이션은 클라이언트에 있고 이것을 서버에 어떻게 동기화 시킬 것인지 고민이 많았습니다. </br>
고정된 위치라면 그냥 애니메이션을 실행시켰겠지만 그것이 아닌 이동하는 애니메이션 이여서 문제였습니다. </br>
이동하는 애니메이션을 고정시켜 서버에서 동작하게도 하였지만 너무 어색하여 다른 방법을 찾아야 했습니다. </br>

그래서 해결방법은 아니지만 애니메이션의 단계별로 나누어 A -> B 위치까지 몇초 걸리는지 측정하여 이동 계획을 만들어서 위치를 완전히 동기화 할 수 있었습니다. </br>
이 방법으로 만들긴 하였지만 시간이 많았다면 툴을 만들어 데이터로 추출하여 적용했다면 좋지 않았을까 싶습니다.</br>

<img width="2210" height="1270" alt="image" src="https://github.com/user-attachments/assets/dbf6c801-5dc8-4613-b8db-06b06c82a48d" />

# 버프 디버프

### 설계 목표
플레이어의 버프와 디버프를 관리하는 컴포넌트 구현 </br>

<img src="https://github.com/user-attachments/assets/c94335f4-5bf0-43d9-a82b-4d4f3566eb05" alt="Video Label" width="400" height="300" />

### 버프 및 디버프 적용
버프의 타입으로 해당 버프가 있는지 체크하여 적용합니다 </br>
만약 이동과 같이 속도를 변경시켜야 하는경우 ApplyBuff에서 값을 변경시킵니다. </br>

<img width="2392" height="2258" alt="image" src="https://github.com/user-attachments/assets/83448944-e4be-497f-a982-5e1ac90e80ec" />

<div align="center"> <h2> SNS </h2> </div>

# 채팅

### 설계 목표
유저들과 소통할 수 있는 채팅 구현 </br>

<a href="https://youtu.be/V_tvPMT1-Mk?si=5YUewK7-nNEPqBQU&t=55">
  <img src="https://github.com/user-attachments/assets/b99b5e6d-994f-41ce-b270-cdd119cd8db5" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 55초부터 시작)</em>

# 파티

### 설계 목표
파티의 상태를 안정적으로 관리하고 동기화하는 시스템 구현 </br>

<a href="https://youtu.be/V_tvPMT1-Mk?si=Y0_Fd4UF8YmNELK4&t=78">
  <img src="https://github.com/user-attachments/assets/58b00dd4-72b9-49c6-a233-1c34d522975b" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (1분 18초부터 시작)</em>

### 권한 기반 명령 처리
파티장 권한을 체크하여 민감한 작업을 처리하였습니다. </br>

<img width="762" height="167" alt="image" src="https://github.com/user-attachments/assets/e00a7ee0-dd4b-4e2d-bfd3-ac12d5cf55b7" />

### 다양한 테스트
파티 참가 가능 여부를 단순한 인원 체크로 판단하지 않고 </br>
다양한 관점에서 발생 할 수 있는 예외 상황들을 꼼꼼하게 검증하며 </br>
방어적으로 작성할 수 있도록 하였습니다. </br>

<img width="764" height="554" alt="image" src="https://github.com/user-attachments/assets/b89a9e05-b1df-49fa-afbc-6b0042749cdf" />

또한 에러 코드를 클라이언트에게 전달해 피드백하였습니다. </br>

<img width="337" height="127" alt="image" src="https://github.com/user-attachments/assets/d18593e2-ebbd-4cfb-8cc7-cc579b46f1ee" />

# 친구

### 설계 목표
친구의 상태를 관리하며 친구가 게임에 접속 했는지 확인 가능한 시스템 구현 </br>

<a href="https://youtu.be/V_tvPMT1-Mk?si=bAHyILk5hUkYY-zg&t=65">
  <img src="https://github.com/user-attachments/assets/2a318941-19cc-44d6-8e9c-79f4616860fe" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (1분 05초부터 시작)</em>

### 친구 접속 시 알림
게임에 접속 시 친구 리스트를 로드하여 현재 접속중인 친구가 있다면 알리기

<img width="2482" height="1462" alt="image" src="https://github.com/user-attachments/assets/d482d085-d0b3-45fa-a1d0-80fb703d0daf" />


<div align="center"> <h2> 캐릭터 </h2> </div>

# 캐릭터 커스텀

### 설계 목표
플레이어의 캐릭터를 커스텀할 수 있도록 구현 </br>

<a href="https://youtu.be/V_tvPMT1-Mk?si=fdfGr-h5dcJqIC_2&t=19">
  <img src="https://github.com/user-attachments/assets/ad7269f8-ba6b-4674-bd5c-d7a3c85a4b02" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 19초부터 시작)</em>

### 캐릭터 생성
캐릭터를 커스텀한 결과를 서버가 받아 DB에 저장합니다.
<img width="2482" height="1740" alt="image" src="https://github.com/user-attachments/assets/11af23f7-01fd-456b-89e8-89f3eadd81b1" />

### 캐릭터 로드
캐릭터의 정보를 DB를 통해 가져오고 크게 유형을 나누어 보냅니다. </br>
1. 캐릭터 기본 정보 (이름, 레벨, 클래스) </br>
2. 캐릭터 외형 (종족, 스킨 색깔) </br>
3. 캐릭터가 착용한 장비 </br>

<img width="2482" height="2674" alt="image" src="https://github.com/user-attachments/assets/ed6d24ff-acbb-4d88-aa7e-0318e37cad71" />



<div align="center"> <h2> 아이템 </h2> </div>

# 루팅

### 설계 목표
아이템이 드랍되면 해당 아이템을 주울 수 있도록 하고 싶었습니다. </br>

<img src="https://github.com/user-attachments/assets/351a3709-78df-4fbd-b09a-2caab1d6a914" alt="Video Label" width="400" height="300" />

### 아이템 드랍
몬스터가 죽거나 플레이어가 아이템을 떨어뜨린다면 해당 위치에 아이템이 떨어졌다는 패킷을 보내게 됩니다. </br>

하지만 재접속 시 아이템이 유지가 되지 않거나 가시거리에서 벗어났다가 다시 돌아오면 보이지 않았습니다. </br>
그래서 아이템을 떨어뜨릴때 해당 정보를 객체로 담아 월드에 넣어 관리함으로써 유지할 수 있도록 하였습니다. </br>

<img width="307" height="68" alt="image" src="https://github.com/user-attachments/assets/44b345df-1158-4062-9939-7a6c9361e3c4" />

### 아이템 픽업
해당 게임은 탑뷰 게임인데 아이템을 클릭하였을 때 거리에 닿지 않는다고 클라이언트에게 전달하는 것은 뭔가 이상했습니다. </br>
그래서 플레이어의 행동 (공격, 줍기, ...)을 설정하여 내가 이동하고나서 이어서 해야할 행동을 지정하게 하여 </br>

자연스럽게 아이템이 있는 곳까지 이동하여 거리에 닿으면 픽업 할 수 있도록 하였습니다. </br>

<img width="2482" height="1014" alt="image" src="https://github.com/user-attachments/assets/c85fd52d-fb3d-4b72-965f-268ddd56d3df" />

# 장비

### 설계 목표
장비를 장착 및 해제가 가능하고 장비에 따라 스텟이 변화할 수 있도록 구현 </br>

<img src="https://github.com/user-attachments/assets/d5d4367a-baa0-4a8c-b9b5-eeacdd3a3c69" alt="Video Label" width="400" height="300" />

### 장비 스텟 적용
제가 했던 게임을 보았을 때 대부분 아이템을 교채하면 최대값만 증가하는 것을 확인하였습니다.

그래서 장비를 장착 및 해제 또는 로드할 때 마다 장비의 스텟 값을 캐릭터의 최대 스텟에 더하고자 하였습니다.
기존에 캐릭터나 몬스터에 사용하던 스텟이라는 데이터가 있었기에 비교적 간단하게 적용할 수 있었습니다.
사용법은 아이템 코드에 따라 장비의 데이터(스텟)을 얻을 수 있었고 이것을 캐릭터의 최대 스텟에 반복하여 더하였습니다.
<img width="1864" height="97" alt="image" src="https://github.com/user-attachments/assets/bd04d71f-2c87-4d41-85c5-407e3b0446af" />

<img width="1710" height="784" alt="image" src="https://github.com/user-attachments/assets/79a21768-3e41-4f3d-8aab-2d4a40634bf6" />

### 장착 및 해제
장비는 1 ~ 9부터 장착 가능한 위치가 존재합니다.
장착 및 해제할 때 넣을 수 있는지 제거할 수 있는지 확인할 수 있도록 하였습니다. </br>
<img width="1610" height="1194" alt="image" src="https://github.com/user-attachments/assets/93014018-919c-45ec-b58e-1198bfa7db63" />


# 인벤토리

### 설계 목표
그리드 형식의 인벤토리를 제작하여 자유롭게 배치할 수 있도록 구현하였습니다. </br>

<img src="https://github.com/user-attachments/assets/6045c1ad-1ad7-4168-a742-3d2a17c62b08" alt="Video Label" width="400" height="300" />

### 아이템 추가
아이템의 코드를 이용하여 크기를 얻어내고 회전을 확인하여 넣을 수 있는지 없는지 체크하였습니다.
아이템의 변경은 빈번하게 일어날 것이기 때문에 클라이언트에서 엄격하게 검사합니다.
그렇다고 클라이언트를 믿을 수 있는 것은 아니기에 서버에서도 간단한 검사를 하였습니다.

<img width="2422" height="1866" alt="image" src="https://github.com/user-attachments/assets/39de2e3c-a4e7-499b-94eb-b178432346b3" />
<img width="441" height="423" alt="image" src="https://github.com/user-attachments/assets/8a575b6a-7d50-4c76-94e3-4f2bd9e4dea9" />


<div align="center"> <h2> 계정 </h2> </div>

# 회원가입 및 인증

### 설계 목표
최근 회원 가입은 SNS를 많이 사용하기도 하지만 </br>
이메일을 통해 인증하거나 2차 코드로 사용되어 지기에 만들어보고자 하였습니다. </br>

<a href="https://youtu.be/V_tvPMT1-Mk?si=QOYz4I2tYBmWb9ss&t=9">
  <img src="https://github.com/user-attachments/assets/8682c1d0-48fc-4bdd-b104-5729ec97e091" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 09초부터 시작)</em>

### 인증 코드
인증 코드는 C++ 서버에서 보내는 방식과 MS-SQL에서 보내는 방식 두개를 사용해보았습니다.  </br>
SMTP에 대한 글을 보면서 따라서 제작하였습니다.  </br>

데이터 베이스는 MS-SQL에 있는 메일 시스템을 사용하였습니다 </br>
두개의 시스템을 이용하면서 이메일이 잘 보내지지 않아 고생을 했지만 </br>
결국에는 SMTP 설정과 보안 연결(SSL)에 대한 문제가 있었습니다 </br>
네이버는 해결했지만 구글을 계속 다른 방법을 사용해도 되지 않아서 해결하지 못했습니다. </br>

<img width="1041" height="299" alt="image" src="https://github.com/user-attachments/assets/2b9468bd-deec-44ef-8197-bc53b856eeee" />

이후 인증을 통해 완전한 가입을 할 수 있도록 하였습니다. </br>

<img width="531" height="41" alt="image" src="https://github.com/user-attachments/assets/a50c2b3f-614c-4420-940c-03a230990981" />


