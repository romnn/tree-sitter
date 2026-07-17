// This grammar makes the parser merge two stack versions whose predecessor chains are as long as the
// input.
// The `left`/`right` conflict splits the stack into two versions that reach the same parse state,
// but only one of them consumed the external `_marker`, so their scanner states differ and the
// versions cannot merge.
// Every comment is then pushed onto both versions in the same state.
// Consuming `_end_marker` gives both versions the same scanner state, so they merge, and the merge
// walks both comment chains down to their common root.

export default grammar({
  name: 'deep_stack_merge',

  externals: $ => [
    $._marker,
    $._end_marker,
  ],

  extras: $ => [/\s/, $.comment],

  conflicts: $ => [
    [$.left, $.right],
  ],

  rules: {
    program: $ => seq($.head, $._end_marker),

    head: $ => choice(
      seq($.left, 'y', $._marker),
      seq($.right, 'y', 'm'),
    ),

    left: _ => 'x',

    right: _ => 'x',

    comment: _ => /#[^\n]*/,
  },
});
