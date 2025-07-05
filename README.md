<div align="center">
  <!--Header-->
  <h1> Dark Crown : Return of the Lich </h1>
  <h2> 논 타겟팅 액션 MMORPG </h2>
</div>

<div>
  
  ### 📋구현 요약 : </br>
  #### System
  - 전투 시스템
    - [몬스터 인공지능](https://github.com/Apeirogon99/Project_LD_Server) : FSM을 이용한 단순한 인공지능 적용
    - [충돌 감지](https://github.com/Apeirogon99/Project_LD_Server) : KD-Tree를 이용하여 충돌 감지
    - [기본/패턴 공격](https://github.com/Apeirogon99/Project_LD_Server) : Shot -> Target -> Over 순서 적용
  - 동기화
    - [이동 동기화](https://github.com/Apeirogon99/Project_LD_Server) : 데드레커닝을 이용한 동기화
    - [애니메이션 동기화](https://github.com/Apeirogon99/Project_LD_Server) : 애니메이션 배속 또는 생략
  - 최적화
    - [가시거리](https://github.com/Apeirogon99/Project_LD_Server) : 대역폭을 줄이기 위한 가시거리 제작 및 활용
  #### Content
  - 전투
    - [스킬](#스킬) : 설치형, 차징, 대쉬, 패링 스킬 구현
    - [패턴](#패턴) : 3페이즈 보스의 스킬 및 다크 나이트 애니메이션 구현
    - [버프/디버프](#버프-디버프) : 캐릭터의 능력치 버프/디버프와 시야를 가리는 블라인드 구현
  - SNS
    - [채팅](#채팅) : 주변에 있다면 채팅을 볼 수 있는 전체 채팅
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
<img src="https://github.com/user-attachments/assets/c94335f4-5bf0-43d9-a82b-4d4f3566eb05" alt="Video Label" width="400" height="300">

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
<img src="https://github.com/user-attachments/assets/351a3709-78df-4fbd-b09a-2caab1d6a914" alt="Video Label" width="400" height="300">

# 장비
<img src="https://github.com/user-attachments/assets/d5d4367a-baa0-4a8c-b9b5-eeacdd3a3c69" alt="Video Label" width="400" height="300">

# 인벤토리
<img src="https://github.com/user-attachments/assets/6045c1ad-1ad7-4168-a742-3d2a17c62b08" alt="Video Label" width="400" height="300">

<div align="center"> <h2> 계정 </h2> </div>

# 회원가입 및 인증
<a href="https://youtu.be/V_tvPMT1-Mk?si=QOYz4I2tYBmWb9ss&t=9">
  <img src="https://github.com/user-attachments/assets/8682c1d0-48fc-4bdd-b104-5729ec97e091" alt="Video Label" width="400" height="300">
</a>
<br>
🎥 <em>이미지를 클릭하면 영상을 볼 수 있습니다 (0분 09초부터 시작)</em>

