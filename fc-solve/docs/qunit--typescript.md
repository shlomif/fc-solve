I am using [QUnit](https://github.com/qunitjs/qunit/) in an MIT-licensed
project written in part in TypeScript. I have some TS functions that
accept the QUnit as a parameter and wish to type them as its interface
from [the typing](https://github.com/DefinitelyTyped/DefinitelyTyped/tree/master/types/qunit)

E.g:

```typescript
import { QUnit } from "qunit";


export function run_tests(q: QUnit)
{
    q.module((a) => { ... });
}
```

However, when I try, I am getting this error:

```
tsc --project lib/typescript/www/tsconfig.json
src/ts/web-fc-solve-tests.ts(12,23): error TS2306: File '/home/shlomif/progs/fre
ecell/git/fc-solve/fc-solve/site/wml/node_modules/@types/qunit/index.d.ts' is no
t a module.
```

Here is my code (note the branch): https://github.com/shlomif/fc-solve/tree/qunit-typescript - it is under the `fc-solve/site/wml` sub directory.

My `tsconfig.json` is:

```json
{
  "compilerOptions": {
      "baseUrl": ".",
      "esModuleInterop": true,
      "module": "amd",
      "moduleResolution": "node",
      "noImplicitReturns": true,
      "outDir": "../../../lib/out-babel/js",
      "paths": {
          "big-integer": ["node_modules/big-integer/BigInteger.d.ts"],
          "qunit": ["node_modules/@types/qunit"]
      },
      "target":"es6"
  },
  "include": ["../../../src/ts/**/*.ts"]
}
```

Thanks for any help.
