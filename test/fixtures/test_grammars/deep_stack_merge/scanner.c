#include "tree_sitter/alloc.h"
#include "tree_sitter/parser.h"

enum {
  MARKER,
  END_MARKER,
};

// The last external token this scanner produced.
// Serializing it is what keeps the two stack versions apart until both consume `_end_marker`.
typedef struct {
  uint8_t last_token;
} Scanner;

void *tree_sitter_deep_stack_merge_external_scanner_create() {
  Scanner *scanner = ts_malloc(sizeof(Scanner));
  scanner->last_token = 0;
  return scanner;
}

void tree_sitter_deep_stack_merge_external_scanner_destroy(void *payload) {
  ts_free(payload);
}

unsigned tree_sitter_deep_stack_merge_external_scanner_serialize(void *payload, char *buffer) {
  Scanner *scanner = payload;
  buffer[0] = (char)scanner->last_token;
  return 1;
}

void tree_sitter_deep_stack_merge_external_scanner_deserialize(
  void *payload,
  const char *buffer,
  unsigned length
) {
  Scanner *scanner = payload;
  scanner->last_token = length > 0 ? (uint8_t)buffer[0] : 0;
}

bool tree_sitter_deep_stack_merge_external_scanner_scan(
  void *payload,
  TSLexer *lexer,
  const bool *valid_symbols
) {
  Scanner *scanner = payload;

  while (lexer->lookahead == ' ' || lexer->lookahead == '\t' ||
         lexer->lookahead == '\n' || lexer->lookahead == '\r') {
    lexer->advance(lexer, true);
  }

  if (valid_symbols[MARKER] && lexer->lookahead == 'm') {
    lexer->advance(lexer, false);
    scanner->last_token = 1;
    lexer->result_symbol = MARKER;
    return true;
  }

  if (valid_symbols[END_MARKER] && lexer->lookahead == 'e') {
    lexer->advance(lexer, false);
    scanner->last_token = 2;
    lexer->result_symbol = END_MARKER;
    return true;
  }

  return false;
}
