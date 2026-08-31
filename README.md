本项目是栈式虚拟机汇编解析器底层源码，是**作者自研的两门编程语言**的核心运行支撑模块，这两门编程语言目前仍在开发当中，未来会正式制作发布。
本解析器内置完整的编译处理逻辑与虚拟机运行调度逻辑，专门用于解析、加载、执行这两门自研编程语言编译产出的汇编字节码文件，同时原生支持字节码持久化保存、字节码动态解释执行全套底层能力，是两门自研编程语言的虚拟机运行基座。

## 开源许可与使用约束

> 完整法律许可请查阅仓库根目录下的 [`LICENSE-CH`](LICENSE-CH)（中文）与 [`LICENSE`](LICENSE)（English）文件。

- 允许：任何人阅读、学习、修改源代码；免费公开分发源码、修改衍生版本、编译后的二进制产物；内部非分发场景自由使用。
  
- 禁止：严禁以任何形式售卖、倒卖、收取费用授权本项目源码、二次修改源码、编译后的可执行程序。所有对外分发版本必须永久免费公开，不允许收费传播。
  
> 注：两门自研编程语言官方完整版配套解释器可能保留独立版权，仅当前 MGVM 开源版本适用上述许可。

---

This project is a stack‑based virtual machine assembler interpreter, serving as the underlying runtime foundation for **two programming languages independently developed by the author**. These two languages are currently under development and will be produced in the future.

This interpreter implements full compilation logic and virtual‑machine scheduling. It can parse, load and execute assembly bytecode emitted by these two self‑developed languages, and natively supports byte‑code persistence and dynamic interpretation.

## License & Usage Restrictions

> For full legal terms, please refer to [`LICENSE`](LICENSE) (English) and [`LICENSE-CH`](LICENSE-CH) (Chinese) in repository root.

- Permitted: Read, study, modify source code; publicly redistribute source code, derivatives and compiled binaries free of charge; internal private usage is allowed.
  
- Prohibited: Selling, reselling or charging for this source code, modified derivatives or compiled executables in any form. All public distributions must remain free‑of‑charge.
  
> Note: The official full‑version interpreters bundled with the two self‑developed languages may carry separate copyrights. Only this open‑source MGVM release is governed by the above‑mentioned license.
