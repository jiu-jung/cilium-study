# Net Namespace 분리하기

Go로 구현한 HTTP API 서버입니다.  
`/unshare/netns` API를 통해 자식 프로세스를 생성하고, 해당 자식 프로세스만 새로운 network namespace에서 실행합니다.

<br>


## 실행 방법
```bash
make build
sudo ./app
make clean
```
- api 예시는 `api.http` 파일에 작성되어 있습니다.

<br>

## API

| 항목 | 값 |
| --- | --- |
| Method | `POST` |
| Path | `/unshare/netns` |
| Listen Port | `8080` |
| Content-Type | `application/json` |

<br>

### Request Body

```json
{
  "path": "/bin/sleep",
  "args": ["30"]
}
```

| 필드     | 설명               |
| ------ | ---------------- |
| `path` | 실행 파일의 절대 경로     |
| `args` | 실행 파일에 전달할 인자 목록 |

- path는 반드시 절대 경로여야 합니다.
- args는 shell command string이 아닌 인자 배열이어야 합니다.
- 예: "/bin/sleep", ["30"]
- 비허용 예: "sleep 30"

<br>

### Response Body

```JSON
{
  "parent_pid": 12345,
  "child_pid": 12346
}
```
| 필드           | 설명                                         |
| ------------ | ------------------------------------------ |
| `parent_pid` | HTTP API 서버 프로세스의 PID                      |
| `child_pid`  | 새로운 network namespace에서 실행 중인 자식 프로세스의 PID |

<br>

## 동작 방식

- `/unshare/netns`는 POST 요청을 받으면 자식 프로세스를 생성합니다.
- 자식 프로세스는 `unshare(CLONE_NEWNET)`를 통해 새로운 network namespace로 분리됩니다.
- network namespace 분리는 자식 프로세스에만 적용됩니다. 부모 HTTP API 서버의 network namespace는 변경되지 않습니다.
- 자식 프로세스는 request body의 path를 args와 함께 실행합니다.
- 부모 프로세스는 자식 프로세스 종료 후 wait 처리를 수행하여 zombie process를 방지합니다.