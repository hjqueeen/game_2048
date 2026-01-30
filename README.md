# 2048 (C++ / SFML)

C++와 SFML로 만든 2048 게임입니다.

## 요구 사항

- **CMake** 3.16 이상
- **C++17** 지원 컴파일러
- **SFML 2.5** 이상 (graphics, window, system)

### SFML 설치

**macOS (Homebrew):**
```bash
brew install sfml
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install libsfml-dev
```

**Windows:**  
[SFML 공식 사이트](https://www.sfml-dev.org/download.php)에서 다운로드 후, CMake에 `SFML_DIR` 또는 `CMAKE_PREFIX_PATH`를 설정하세요.

## 빌드 및 실행

```bash
cd game_2048
cmake -B build
cmake --build build
./build/game_2048
```

## 조작법

- **화살표 키**: 타일 이동 (위/아래/좌/우)
- **R**: 게임 다시 시작 (게임 오버 후)

## 규칙

- 같은 숫자 타일이 부딪히면 합쳐집니다 (예: 2 + 2 → 4).
- 매 이동 후 빈 칸 하나에 2 또는 4 타일이 생성됩니다.
- 더 이상 움직일 수 없으면 게임 오버입니다.
- 2048 타일을 만들면 승리입니다 (현재 버전은 게임 오버 시에만 재시작 가능).
