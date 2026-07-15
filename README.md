# ConsoleMVC-Chan-0613

반도체 시료 생산주문관리 시스템(SampleOrderSystem) 본 프로젝트에 앞서 진행하는 **MVC 스켈레톤 PoC**입니다.
Model / View / Controller 계층을 명확히 분리한 C++20 콘솔 애플리케이션으로, 이후 진행되는 다른 PoC 및
최종 프로젝트가 이 골격을 그대로 가져다 확장할 수 있도록 설계했습니다.

## 이 저장소의 위치

과제는 아래 5개 저장소로 나뉘어 있으며, 이 저장소는 그중 **MVC 스켈레톤 코드**를 담당합니다.

| PoC 항목 | 저장소 | 비고 |
|---|---|---|
| MVC 스켈레톤 코드 | **ConsoleMVC-Chan-0613 (본 저장소)** | 계층 구조 / Repository 인터페이스 확정 |
| 데이터 영속성 처리 (JSON) | DataPersistence-Chan-0613 | 이 저장소의 Repository 인터페이스를 JSON 구현체로 대체 |
| 데이터 모니터링 Tool | DataMonitor-Chan-0613 | 이 저장소의 Model 조회 메서드를 재사용 |
| Dummy 데이터 생성 Tool | DummyDataGenerator-Chan-0613 | 이 저장소의 Repository 등록 메서드를 재사용 |
| 반도체 시료 생산주문관리 시스템 | SampleOrderSystem-Chan-0613 | 위 4가지를 조립한 최종 프로젝트 |

이 저장소는 **In-Memory 데이터만 사용**하며, 파일/DB 영속성·실시간 모니터링 도구·더미 데이터 생성기는
직접 구현하지 않습니다. 대신 다른 PoC가 그대로 이어붙일 수 있는 인터페이스와 접점을 확정하는 데 집중합니다.
자세한 설계 배경은 [CLAUDE.md](./CLAUDE.md)를 참고하세요.

## 주요 기능

콘솔 메인 메뉴에서 아래 기능을 제공합니다. (내부 로직은 PoC 수준으로 단순화되어 있으나, 최종 시스템의
메뉴/흐름을 그대로 반영합니다.)

| 메뉴 | 기능 |
|---|---|
| 1. 시료 관리 | 시료 등록 / 목록 조회 / 이름 검색 |
| 2. 시료 주문 | 고객 주문 예약 접수 (`RESERVED`) |
| 3. 주문 승인/거절 | 예약 목록 확인 후 승인(`CONFIRMED`/`PRODUCING`) 또는 거절(`REJECTED`) |
| 4. 모니터링 | 상태별 주문 수, 시료별 재고 현황(여유/부족/고갈) |
| 5. 생산 라인 조회 | 생산 중인 시료 및 FIFO 대기 큐 조회, 생산 완료 처리(시뮬레이션) |
| 6. 출고 처리 | `CONFIRMED` 주문 출고(`RELEASED`) |

### 도메인 계산 규칙

- 실 생산량 = `ceil(부족분 / 수율)`
- 총 생산 시간 = `평균 생산시간 × 실 생산량`
- 주문 상태 흐름: `RESERVED` → (`PRODUCING` | `CONFIRMED`) → `RELEASED`, 이탈 상태 `REJECTED`

## 아키텍처

```
ConsoleMVC-Chan-0613/
├── Model/
│   ├── Sample.h / .cpp                    # 시료 엔티티
│   ├── Order.h / .cpp                     # 주문 엔티티, OrderStatus enum
│   ├── ProductionLine.h / .cpp            # 생산 라인 / FIFO 생산 큐
│   ├── Dtos.h                             # Controller ↔ View 공용 결과 타입
│   └── Repository/
│       ├── ISampleRepository.h / IOrderRepository.h   # 인터페이스
│       └── InMemorySampleRepository / InMemoryOrderRepository  # In-Memory 구현체
├── View/          # 콘솔 입출력 전담 (메뉴 출력, 입력 수신, 결과 렌더링)
├── Controller/    # View 입력을 받아 Model을 조작하고 결과를 View에 전달
└── main.cpp       # Repository 구현체 선택 및 조립(DI), 메뉴 루프
```

- **Model**: 도메인 데이터와 비즈니스 규칙만 담당하며 콘솔 입출력을 직접 호출하지 않습니다.
- **View**: 콘솔 입출력만 담당하며 비즈니스 로직을 포함하지 않습니다.
- **Controller**: Model과 View를 중개합니다. Model과 View는 서로 직접 참조하지 않습니다.
- Repository는 인터페이스로 분리되어 있어, `main.cpp`에서 구현체를 교체하는 것만으로 다른 PoC(JSON 영속성 등)와
  통합할 수 있습니다.

## 기술 스택

- C++20 (`/std:c++20`)
- Visual Studio / MSBuild (`vcxproj`), Windows Console Application (Win32 / x64, Debug / Release)
- 외부 의존성 없음 (표준 라이브러리만 사용)

## 빌드 및 실행

### Visual Studio

1. `ConsoleMVC-Chan-0613.slnx`를 Visual Studio 2022로 엽니다.
2. 구성/플랫폼을 선택 후 빌드(Ctrl+Shift+B) → 실행(F5 또는 Ctrl+F5)합니다.
   - 프로젝트의 `PlatformToolset`이 로컬에 설치된 버전과 다르면, 솔루션 우클릭 → **솔루션 대상 변경**으로 맞춰주세요.

### 명령줄(MSBuild)

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
  ConsoleMVC-Chan-0613.slnx /p:Configuration=Debug /p:Platform=x64

.\x64\Debug\ConsoleMVC-Chan-0613.exe
```

> 소스 파일은 한글 주석을 포함하므로 컴파일 시 `/utf-8` 옵션이 적용되어 있습니다(프로젝트 설정에 반영됨).

## 스코프 (이번 PoC 기준)

포함:
- Model / View / Controller 계층 분리 및 폴더 구조
- 전체 메뉴에 대한 Controller 메서드와 View 화면
- Repository 인터페이스 + In-Memory 구현체
- 상태 전이 및 재고/생산량 계산 로직의 Model 내 캡슐화

제외 (다른 PoC 저장소의 책임):
- JSON 파일 실제 읽기/쓰기 (인터페이스와 필드↔JSON 키 매핑만 확정)
- 실시간 모니터링 도구 UI
- 더미 데이터 자동 생성 로직

자세한 설계 원칙과 각 PoC 간 통합 접점은 [CLAUDE.md](./CLAUDE.md)에 정리되어 있습니다.
