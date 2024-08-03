macro: friend_method
headers: yorel/yomm2/cute.hpp, yorel/yomm2/keywords.hpp

macro: YOMM2_FRIEND
headers: yorel/yomm2/macros.hpp, yorel/yomm2.hpp

```
#define friend_method(container)
#define friend_method(container, return_type, name, (function-parameter-list))
```

Grant friendship to all the methods inside a container friend of a class, or to
a specific method.

## Example
See the [containers](https://github.com/jll63/yomm2/tree/master/examples/containers) example.
