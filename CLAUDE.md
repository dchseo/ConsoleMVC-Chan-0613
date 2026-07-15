# ConsoleMVC-Chan-0613

## 프로젝트 목적

"반도체 시료 생산주문관리 시스템(SampleOrderSystem)" 최종 프로젝트에 앞서 진행하는 PoC(Proof of Concept) 저장소.
이 저장소의 목표는 단순히 "Model/View/Controller 폴더가 나뉘어 있다"를 보여주는 것이 아니라,
**최종 프로젝트가 이 골격을 그대로 가져다 확장할 수 있는 수준의 설계**를 만드는 것이다.

즉 이후 4개 저장소가 통합되는 흐름은 다음과 같다.

```
ConsoleMVC        → 계층 구조 / 인터페이스 확정      (본 저장소)
DataPersistence    → Repository 인터페이스의 실제 구현체를 JSON 파일 기반으로 제공
DataMonitor        → Model(Repository)을 읽어 콘솔에 실시간 표시
DummyDataGenerator → Repository에 테스트 데이터 주입
SampleOrderSystem  → 위 4가지를 조립한 최종본, 기능 명세의 세부 로직을 완성
```

데이터 영속성 방식은 **JSON**으로 확정되어 있다. 따라서 이 저장소의 도메인 모델과 Repository 인터페이스는
"JSON으로 직렬화/역직렬화하기 쉬운 형태"를 최우선으로 설계한다 (자세한 내용은 아래 "JSON 영속성 연계 설계" 참고).

이 저장소에서 내리는 설계 결정(클래스 이름, 인터페이스 시그니처, 폴더 구조, 상태값 이름)은
**다른 PoC 및 최종 프로젝트에서 그대로 재사용되는 것을 전제로** 정한다. 이름을 바꾸거나 구조를
뒤엎어야 한다면 통합 비용이 커지므로, "나중에 무엇이 꽂힐 자리인지"를 지금 결정한다는 마음으로 설계한다.

과제 안내상 PoC는 아래 4종으로 분리되어 있으며, 이 저장소는 그중 "MVC 스켈레톤 코드"만 담당한다.

| PoC 항목 | 저장소 | 이 저장소 범위 |
|---|---|---|
| MVC 스켈레톤 코드 | **ConsoleMVC-Chan-0613 (본 저장소)** | ✅ 담당 |
| 데이터 영속성 처리 (JSON) | DataPersistence-Chan-0613 | ❌ 구현은 아님, 단 **Repository 인터페이스 및 JSON 직렬화 규약은 여기서 정의** |
| 데이터 모니터링 Tool | DataMonitor-Chan-0613 | ❌ 구현은 아님, 단 Model 조회 접점(Getter)은 여기서 확정 |
| Dummy 데이터 생성 Tool | DummyDataGenerator-Chan-0613 | ❌ 구현은 아님, 단 등록 접점(Repository.add)은 여기서 확정 |
| 반도체 시료 생산주문관리 시스템 | SampleOrderSystem-Chan-0613 | ❌ 최종 통합, 이 저장소의 클래스를 그대로 계승/확장 |

이 저장소에서는 **파일/DB 영속성, 실시간 모니터링 도구, 더미 데이터 생성기를 직접 구현하지 않는다.**
대신 In-Memory 구현체 하나로 그 자리(인터페이스)를 채워 넣어 동작을 검증하고, 다른 PoC가 같은 인터페이스의
또 다른 구현체가 되도록 설계한다.

## 설계 철학: "교체 가능한 자리"를 먼저 만든다

- **Repository는 반드시 인터페이스(추상 클래스)로 선언**하고, 이 PoC에서는 `InMemory` 구현체 하나만 둔다.
  DataPersistence PoC는 같은 인터페이스의 `JsonRepository` 구현체를 만들면 되고, SampleOrderSystem은 생성
  시점에 어떤 구현체를 주입할지만 결정하면 된다. (의존성 주입: 생성자에서 `ISampleRepository&` 를 받는 형태)
- **Repository 인터페이스는 파일 I/O를 전제하지 않되, 저장/로드 시점이 명확한 형태로 설계**한다. 예를 들어
  `Add`/`Update`/`Remove` 같은 변경 메서드가 호출될 때마다 즉시 저장할지, `Save()`/`Load()`를 별도로 둘지는
  DataPersistence PoC가 결정할 문제이므로, 인터페이스에는 두 방식 모두를 막지 않는 최소한의 메서드만 둔다.
- **DummyDataGenerator가 붙을 자리**: Repository의 `Add(...)` 계열 public 메서드가 곧 더미 데이터 주입 지점이다.
  이 메서드의 시그니처가 이 PoC에서 확정되면, DummyDataGenerator는 별도 실행 파일에서 같은 Repository를
  호출하기만 하면 된다.
- **DataMonitor가 붙을 자리**: Repository/Model의 조회 메서드(`GetAll`, `FindById`, `GetStatusSummary` 등)가
  곧 모니터링 도구의 데이터 소스다. Controller를 거치지 않고 Model을 직접 읽기만 해도 되도록, 조회 메서드는
  부작용 없이(side-effect free) 설계한다.
- **상태값/도메인 용어는 기능 명세서의 표기를 그대로 사용**한다 (`RESERVED`, `REJECTED`, `PRODUCING`,
  `CONFIRMED`, `RELEASED` 등). 나중에 이름을 다시 맞추는 리네이밍 비용을 없앤다.

## 기능 명세 (최종 프로젝트 기준, 이 PoC는 구조만 선반영)

최종 시스템의 메인 메뉴와 기능은 아래와 같다. 이 PoC에서는 각 메뉴에 대응하는 Controller 메서드와
View 화면을 **스텁 수준이라도 전부 만들어 둔다.** (내부 로직은 단순화해도 되지만, 시그니처와 호출 흐름은
최종본에서 그대로 살릴 수 있어야 한다.)

| 메뉴 | 기능 | 담당 Controller 메서드(예) | 비고 |
|---|---|---|---|
| 1. 시료 관리 | 시료 등록 / 목록 조회 / 이름 검색 | `RegisterSample`, `ListSamples`, `SearchSample` | 속성: ID, 이름, 평균생산시간, 수율 |
| 2. 시료 주문 | 고객 주문 예약 접수 | `ReserveOrder` | 입력: 시료ID, 고객명, 수량 → 상태 `RESERVED` |
| 3. 주문 승인/거절 | RESERVED 목록 확인 후 승인/거절 | `ApproveOrder`, `RejectOrder` | 승인시 재고 비교 후 `CONFIRMED`/`PRODUCING` 분기 |
| 4. 모니터링 | 상태별 주문 수 / 시료별 재고 현황 | `GetOrderStatusSummary`, `GetInventoryStatus` | `REJECTED`는 집계 제외 |
| 5. 생산 라인 조회 | 생산 중 시료 / 대기 큐(FIFO) 조회 | `GetCurrentProduction`, `GetProductionQueue` | 완료시 `PRODUCING → CONFIRMED` |
| 6. 출고 처리 | CONFIRMED 주문 출고 | `ReleaseOrder` | 완료시 `CONFIRMED → RELEASED` |

도메인 계산 규칙(최종본에서 사용, 이 PoC에서는 Model에 캡슐화해 두면 재사용 가능):
- 실 생산량 = `ceil(부족분 / 수율)`
- 총 생산 시간 = `평균 생산시간 * 실 생산량`
- 재고 상태 분류: 여유 / 부족 / 고갈(0)

## 도메인 모델

- **Sample(시료)**: `SampleId`, `Name`, `AvgProductionTime`, `Yield`, `Stock`
- **Order(주문)**: `OrderId`, `SampleId`, `CustomerName`, `Quantity`, `Status`
  - `OrderStatus`: `RESERVED` → (`PRODUCING` | `CONFIRMED`) → `RELEASED`, 이탈 상태 `REJECTED`
- **ProductionLine(생산 라인)**: 단일 라인, FIFO 생산 큐(`ProductionQueue`)로 부족분을 순차 생산

## JSON 영속성 연계 설계

DataPersistence PoC가 JSON으로 그대로 얹을 수 있도록, 이 저장소에서 아래 규약을 미리 확정해 둔다.

- **필드 ↔ JSON 키 매핑을 지금 확정**한다 (DataPersistence에서 임의로 다시 정하지 않도록).
  - Sample → `{ "sampleId": string, "name": string, "avgProductionTime": number, "yield": number, "stock": number }`
  - Order → `{ "orderId": string, "sampleId": string, "customerName": string, "quantity": number, "status": string }`
    - `status`는 enum을 문자열(`"RESERVED"` 등)로 직렬화 — 사람이 읽는 JSON 파일에서도 상태를 바로 식별 가능해야 함
  - 키 네이밍 컨벤션: camelCase 고정 (C++ 멤버는 기존 컨벤션 유지, 직렬화 시에만 camelCase로 매핑)
- **엔티티는 JSON 친화적으로 단순하게 설계**한다 (POD에 가깝게): 포인터/참조를 멤버로 갖지 않고, 값 타입만
  사용한다. `ProductionLine`의 큐처럼 관계(주문 ID 참조)는 ID 문자열로만 연결하고 객체 참조로 들고 있지 않는다.
- **직렬화 책임은 Model이 아니라 Repository(구현체)에 둔다.** 이 PoC의 In-Memory 구현체는 직렬화를 하지
  않지만, 엔티티에 `ToJson()`/`FromJson()`류의 변환 편의 메서드를 두면 DataPersistence가 그대로 재사용할 수
  있다. 이 메서드가 존재하더라도 Model이 파일 시스템을 알 필요는 없다 (직렬화 ↔ 파일 저장은 분리).
- **외부 JSON 라이브러리는 이 저장소에서 도입하지 않는다** (스코프 외). 다만 DataPersistence PoC는
  `nlohmann/json` 채택을 전제로 엔티티를 설계해 둔다 — 즉 `ToJson()`이 반환할 타입은 이 라이브러리의
  `json` 객체와 자연스럽게 호환되는 형태(문자열 키, 기본 타입 값)로 남겨 둔다.
- **저장 단위는 Repository 전체 컬렉션 하나당 JSON 파일 하나**를 기본 가정으로 한다
  (예: `samples.json`, `orders.json`). 이 PoC의 폴더 구조에서 파일 경로 개념 자체는 등장하지 않지만,
  Repository 인터페이스가 "컬렉션 단위 로드/저장"을 자연스럽게 표현할 수 있어야 한다.

## 기술 스택

- 언어: C++20 (`stdcpp20`)
- 빌드: Visual Studio (MSBuild, vcxproj), PlatformToolset v145
- 대상: Windows Console Application (Win32 / x64, Debug / Release)
- 외부 의존성 없음 (표준 라이브러리만 사용)

## 아키텍처 — MVC 계층 분리

```
ConsoleMVC-Chan-0613/
├── Model/
│   ├── Sample.h / .cpp                    # 시료 엔티티
│   ├── Order.h / .cpp                     # 주문 엔티티, OrderStatus enum
│   ├── ProductionLine.h / .cpp            # 생산 라인 / FIFO 생산 큐
│   └── Repository/
│       ├── ISampleRepository.h            # 인터페이스 — DataPersistence가 JsonSampleRepository로 교체할 자리
│       ├── IOrderRepository.h             # 인터페이스 — DataPersistence가 JsonOrderRepository로 교체할 자리
│       ├── InMemorySampleRepository.h/.cpp
│       └── InMemoryOrderRepository.h/.cpp
├── View/
│   ├── MainMenuView.h / .cpp
│   ├── SampleView.h / .cpp
│   ├── OrderView.h / .cpp
│   ├── MonitoringView.h / .cpp
│   └── ProductionLineView.h / .cpp
├── Controller/
│   ├── SampleController.h / .cpp
│   ├── OrderController.h / .cpp           # 예약/승인/거절/출고 포함
│   └── MonitoringController.h / .cpp
└── main.cpp                               # Repository 구현체 선택 및 조립(DI)
```

### 역할 원칙

- **Model**: 도메인 데이터와 비즈니스 규칙(재고 계산, 상태 전이, 생산량 계산)만 담당. 콘솔 입출력을 직접
  호출하지 않는다. Repository는 인터페이스에 의존하고, 구현체(In-Memory)는 별도 클래스로 분리한다.
- **View**: 콘솔 입출력만 담당(메뉴 출력, 입력 수신, 결과 렌더링). 비즈니스 로직·상태 판단을 포함하지 않는다.
- **Controller**: View의 입력을 받아 Repository 인터페이스를 통해 Model을 조작하고, 결과를 View에 전달한다.
  Model과 View는 서로 직접 참조하지 않고 반드시 Controller를 경유한다.
- **main.cpp**: 어떤 Repository 구현체를 사용할지 결정해 Controller에 주입하는 유일한 위치. 이 지점만 바꾸면
  최종 프로젝트에서 DataPersistence의 구현체로 교체 가능해야 한다.
- 순환 의존 금지: `Model → View/Controller` 방향의 include는 금지.

## 이번 PoC의 스코프

포함:
- Model / View / Controller 폴더 구조와 헤더/소스 분리
- 기능 명세 표에 나열된 메뉴 전체에 대한 Controller 메서드 시그니처와 View 화면 스텁
- Sample / Order / ProductionLine의 Repository 인터페이스 정의 + In-Memory 구현체
- 상태 전이(`RESERVED → PRODUCING/CONFIRMED → RELEASED`, `REJECTED`) 로직의 Model 내 캡슐화
- 재고/생산량 계산 규칙(수율, ceil, FIFO)의 Model 내 캡슐화 — 최종본에서 그대로 재사용

제외 (다른 PoC 저장소의 책임, 단 접점은 이 저장소에서 확정):
- JSON 파일 실제 읽기/쓰기 구현 (인터페이스와 필드↔JSON 키 매핑, `nlohmann/json` 채택 전제만 확정)
- 실시간 데이터 모니터링 관리자 도구의 UI/주기적 갱신 (Model 조회 메서드만 확정)
- 더미 데이터 자동 생성 로직 자체 (Repository 등록 메서드만 확정)

## 커밋/문서 관리

- Agentic Engineering 평가 기준(문서 관리, Harness, Test, CleanCode, Commit 이력)을 고려하여 작업 단위로
  의미 있는 커밋을 남긴다.
- 이 파일(CLAUDE.md)은 스코프가 바뀌거나 구조가 변경될 때 함께 갱신한다.
- 인터페이스(Repository, 메서드 시그니처)를 변경할 경우, 다른 3개 PoC와의 통합 지점이 깨질 수 있으므로
  변경 사유를 커밋 메시지에 명시한다.
