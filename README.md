# The Legend of Zelda: Breath of the Wild
🏹 3D Action Adventure<br>

| 게임 명 | 젤다의 전설: 야생의 숨결 모작 |
| --- | --- |
| 게임 장르 | 액션 어드벤처 |
| 플랫폼 | PC (Windows) |
| 개발 인원 | 1인 |
| 사용 엔진 | Unreal Engine 5.6.1 |
| 프로젝트 기간 | 2026.03.24 ~ 2026.04.06 (14일)) |
| 배포 | [구글 드라이브 링크](https://drive.google.com/file/d/1QUrdtpGDwYDxwX2RkqwOZbQwVYEdehxF/view?usp=sharing) |
| 개발 내용 | [노션 링크](https://jengdeuk.notion.site/33d5d8590d11808d85a1e6a3c823eda6) |

## 게임 소개

<aside>
💡 원티드 포텐업에서 진행하는 게임 개발자 양성과정 4th프로그램에서 제작한 게임입니다. <br>
Unreal Engine 5와 Blueprint 학습을 목적으로 Nintendo의 젤다의 전설: 야생의 숨결을 모작했습니다.
</aside>

## 게임 영상

[유튜브 링크](https://youtu.be/dqtvNOARaIw?si=L4lqzuxHv_cVjJon)

## 개발 내용

[노션 링크](https://jengdeuk.notion.site/33d5d8590d11808d85a1e6a3c823eda6)
1. 플레이어 활 조준 애니메이션 - IK 움직임, 상/하체 분리
2. 적 총구 조준 애니메이션 - Bone Modify
3. 카툰 스타일 렌더링 - 셀 셰이딩, 림 라이트
4. 머리카락, 천 시뮬레이션 - 클로딩
5. 시네마틱 연출 - 레벨 시퀀스
6. 보스 AI 작업 - BehaviorTree, FSM

## 리소스 출처

- 3D 모델: https://models.spriters-resource.com/wii_u/thelegendofzeldabreathofthewild/
- 3D 애니메이션: https://www.mixamo.com/
- UI 텍스처: http://figma.com/community/file/965825767811358609
- VFX, 환경, 구조물: http://fab.com/

## 보완할 점

- 전투, AI, 연출 등 다양한 기능을 개별적으로 구현하는 과정에서 기능 간 결합도가 높아지고 재사용성이 떨어지는 구조가 형성되었습니다. 이를 통해 기능 단위로 책임을 분리하고, 인터페이스와 컴포넌트 기반 설계를 적용하여 새로운 기능 추가 시 기존 코드를 수정하지 않도록 구조를 개선할 필요성을 느꼈습니다.
- 일부 로직이 Tick에 의존하고 불필요한 연산이 반복되는 구조로 인해 성능 최적화의 필요성을 느꼈습니다. 이후에는 업데이트 주기 관리, 조건 기반 실행, 그리고 핵심 로직의 C++ 전환 등을 통해 성능을 개선할 수 있도록 보완하고자 합니다.

## 프로젝트 성과

- 다양한 시스템(IK, AI, 연출, 렌더링)을 블루프린트 기반으로 통합하여 실제 플레이 가능한 프로토타입을 구현했으며, 시스템 간 연동 과정에서 발생하는 문제를 해결하며 게임 클라이언트 전반에 대한 이해도와 구조 설계 역량을 향상시켰습니다.
