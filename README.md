# 우리가 만든 디버그 프린터 우리가 쓰자

* **"자쓰가리우터린프그버디든만가리우"** - *invrtd.h*

구조체에 단 한 줄의 코드를 추가하는 것만으로, 구조체를 string으로 바꿀 수 있게 해 주는 라이브러리입니다.

이 라이브러리는 다음과 같은 설계를 지향합니다.

* **절대로 유저에게는 보여주면 안 되는 못생긴 디버그 메시지**
* **최소한의 헤더만 추가하는 가벼운 라이브러리**

### 예제 입력

```cpp
#include "ugly_print.h"

struct MyStruct {
    int x, y;
    char c, d;
    
    // you need to write only this line to print
    constexpr static const char* 
    mem_names[] = {"x_val", "y_val", "c_val", "d_val"};
};

struct MyStruct2 {
    int x, y;
    const char *name;
    
    // you need to write only this line to print
    constexpr static int mem_num = 3;
};

int main() {
    auto my_instance = MyStruct{1, 2, 'U', 'K'};
    std::cout << ugly::decay_to_str(my_instance) << '\n';
    
    auto my_instance2 = MyStruct2{6, 9, "instance 2"};
    std::cout << ugly::decay_to_str(my_instance2) << '\n';
}
```

### 예제 출력
```
(x_val: 1, y_val: 2, c_val: U, d_val: K)
(6, 9, instance 2)
```

### 제한
구조체 원소의 개수가 **10개** 초과여서는 안 됩니다.

