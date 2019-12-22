'use strict';
const { test } = require('tap');

const startCLI = require('./start-cli');

test('qunit-emcc-fcs', (t) => {
    const cli = startCLI(['node_modules/.bin/qunit', 'lib/for-node/test-code-emcc.js']);

    function onFatal(error) {
        cli.quit();
        throw error;
    }
    const timeout = 20*1000;

    class Queue {
        constructor() {
            const that = this;
            that.idx = 0;
            that.cmds =['sb("lib/for-node/js/web-fc-solve-tests.js", 2)',
'sb("lib/for-node/js/libfreecell-solver.min.js", 1833)',
            'cont',
        ];
        };
        run() {
            const that = this;
            cli.command(that.cmds[that.idx++]) .then(() => cli.waitForPrompt(timeout)) .then(() => {console.log(cli.output);}) .then(() => {that.run()});
        }
    }

    const queue = new Queue();

    return cli.waitForInitialBreak(timeout)
        .then(() => cli.waitForPrompt(timeout))
//        .then(() => {queue.run();})
        .then(() => {console.log(cli.output);})
        .then(() => cli.quit())
        .then(null, onFatal);
});

