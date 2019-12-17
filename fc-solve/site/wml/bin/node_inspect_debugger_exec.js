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

  return cli.waitForInitialBreak(timeout)
    .then(() => cli.waitForPrompt(timeout))
/*    .then(() => cli.command('exec [typeof heartbeat, typeof process.exit]'))
    .then(() => {
      t.match(cli.output, '[ \'function\', \'function\' ]', 'works w/o paren');
    }) */
    /*.then(() => cli.command('repl'))
    .then(() => {
      t.match(
        cli.output,
        'Press Ctrl + C to leave debug repl\n> ',
        'shows hint for how to leave repl');
      t.notMatch(cli.output, 'debug>', 'changes the repl style');
    })*/
    .then(() => cli.command('sb("lib/for-node/js/web-fc-solve-tests.js", 2)'))
    .then(() => cli.command('sb("lib/for-node/js/libfreecell-solver.min.js", 1833)'))
    .then(() => cli.command('cont'))
    .then(() => {console.log(cli.output);})
    // .then(() => cli.waitFor(/debug> $/))
    // .then(() => cli.command('cont'))
    // .then(() => cli.command('cont'))
    // .then(() => cli.waitFor(/function/))
    .then(() => cli.waitForPrompt(timeout))
    .then(() => cli.command('cont'))
    .then(() => cli.waitForPrompt(timeout))
    // .then(() => cli.waitFor(/No|throw/, timeout))
    .then(() => {
        console.log(cli.output);
        // console.log(cli);
    })
    .then(() => cli.waitForPrompt(timeout))
    /*.then(() => {
      t.match(
        cli.output,
        '[ \'function\', \'function\' ]', 'can evaluate in the repl');
      t.match(cli.output, /> $/);
    })
    .then(() => cli.ctrlC())
    .then(() => cli.waitFor(/debug> $/))
    .then(() => cli.command('exec("[typeof heartbeat, typeof process.exit]")'))
    .then(() => {
      t.match(cli.output, '[ \'function\', \'function\' ]', 'works w/ paren');
    })
    .then(() => cli.command('cont'))
    .then(() => cli.command('exec [typeof heartbeat, typeof process.exit]'))
    .then(() => {
      t.match(
        cli.output,
        '[ \'undefined\', \'function\' ]',
        'non-paused exec can see global but not module-scope values');
    })
    */
    .then(() => {console.log(cli.output);})
    .then(() => cli.quit())
    .then(null, onFatal);
});

