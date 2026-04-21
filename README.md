# Terminal Defense: Bug Siege
👾 2D ASCII tower defense<br>
A*, Quad Tree 알고리즘 시각화를 주제로 개발한 콘솔 앱 기반의 2D 타워 디펜스 게임입니다.

| 게임 명 | Terminal Defense: Bug Siege |
| --- | --- |
| 게임 장르 | 타워 디펜스 |
| 플랫폼 | PC (Windows, Console Application) |
| 개발 인원 | 1인 |
| 사용 엔진 | C++ 기반 자체 개발 |
| 프로젝트 기간 | 2026.03.04 ~ 2026.03.10 (7일) |
| 배포 | [구글 드라이브 링크](https://drive.google.com/file/d/1tyiLLqg0sZ9AhpaeeGh0OrTi0a0qdMXr/view?usp=sharing) |
| 개발 내용 | [노션 링크](https://jengdeuk.notion.site/Terminal-Defense-Bug-Siege-3255d8590d11805ca5c4f1dad2f4291f) |

## 게임 소개

<aside>
💡 `원티드 포텐업`에서 진행하는 `게임 개발자 양성과정 4th`프로그램에서 제작한 게임입니다.<br>
A*, Quad Tree 알고리즘 시각화를 주제로, C++ 콘솔 텍스트 출력 기반의 타워 디펜스 게임을 제작했습니다.
</aside>

## 게임 영상

[유튜브 링크](https://youtu.be/iXV6w66aEbA?si=9IWg5skGd28inNqv)

## 개발 내용

[노션 링크](https://jengdeuk.notion.site/Terminal-Defense-Bug-Siege-3255d8590d11805ca5c4f1dad2f4291f)
1. 오브젝트 관리 시스템 - 오브젝트 풀링
2. 적 엘리트 유닛 이동 시스템 - A*
3. 적 일반 유닛 이동 시스템 - Flow Field & BFS
4. 적 엘리트 유닛 목표물 탐지 시스템 - Quad Tree
5. 아군 타워 유닛 목표물 탐지 시스템 - Uniform Grid
6. 유닛 시스템 - FSM

## 보완할 점

- A* 경로 탐색 과정에서 간헐적인 병목 현상이 발생했으나, 정확한 원인을 완전히 규명하지 못했습니다. 향후에는 프로파일링 도구를 활용해 탐색 노드 수, 힙 연산 비용, 메모리 접근 패턴 등을 정량적으로 분석하여 병목 원인을 명확히 파악할 계획입니다.
- 맵의 활용 범위가 중앙 영역에 집중되어 있어 공간 활용도가 낮았으며, 이는 기획 단계에서 플레이 영역 설계가 충분히 고려되지 않은 점에서 비롯되었습니다. 향후에는 시스템 구현과 더불어 맵 설계와의 연계도 함께 고려할 필요성을 느꼈습니다.

## 프로젝트 성과

- A*, Flow Field, QuadTree, Uniform Grid 등 다양한 알고리즘을 학습하고 직접 구현하여, 상황에 따라 적절한 알고리즘을 선택하고 적용하는 경험을 쌓았습니다.
- 특히 경로 탐색과 공간 분할 문제를 해결하는 과정에서 알고리즘의 시간 복잡도와 데이터 구조 선택이 실제 성능에 미치는 영향을 체감할 수 있었습니다.
