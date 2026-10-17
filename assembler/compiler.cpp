#include <assert.h>
#include <cstdint>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../Stack/utility/Utility.h"
#include "../Stack/utility/colours.h"
#include "../Stack/utility/errors.h"
#include "../Stack/utility/log.h"
#include "../processor/asm_uitility.h"
#include "arguments.h"

void unk_com(char *comm, stream *assembler, int64_t *line) {
  assert(comm);
  assert(assembler);
  assert(line);

  printlg("Compiling failure: unknown command: <");
  my_printlg(comm);
  printlg("> in %d line\n", calc_offset(assembler, line));
}

void print_byn(FILE *list, int byte_code_desc, int64_t command, int64_t value,
               char *arg) {
  assert(list);

  write(byte_code_desc, &command, sizeof(int64_t));
  fprintf(list, "%lld ", command);
  fprintf(stderr, HRED "%lld" reset, command);

  if ((command == PUSH) && *arg != ':') {
    fprintf(list, " %lld", value * 1000);
    write(byte_code_desc, &value, sizeof(int64_t));
  } else if (command == PUSH) {
    int count = 0;
    while (*arg != ' ' && *arg != '\n' && *arg != '\0') {
      count++;
      putc(*arg, list);
      arg++;
    }
    write(byte_code_desc, arg - count, sizeof(char) * count);
  }

  fprintf(list, "\n");
}

SPU assemble(char filename[PATH_MAX]) {
  assert(filename);

  //------------Init start--------------
  double value = 0;
  int64_t offset = 0;
  int64_t line = 0;
  int64_t PC = 0;
  SPU spu = {};
  labels lab = {};
  int64_t temp = 0;

  stream assembler = asm_open(filename);
  FILE *ostream_list = fopen("code/list_code.txt", "w");

  if (assembler.desc == -1 || assembler.virt_mem == MAP_FAILED)
    return {.status = Failure};

  SPUasm_init(&spu);

  if (spu.status == Failure)
    return spu;

  if (labels_init(&lab) == Failure)
    return {.status = Failure};

  char *comm = assembler.virt_mem;

  //-----------End of init-------------

  printlg("---| Starting compilation |---\n\n");

  while (my_strcmp(comm, "HLT")) {
    if (offset >= assembler.size) {
      printlg("Assembler failure. offset > assembler size\n");
      fprintf(stderr, "Assembler failure, check log - file.\n");
      return {.status = Failure};
    }

    comm = assembler.virt_mem + offset;

    if (!my_strcmp(comm, "PUSH")) {
      move_offset(&assembler, &offset, &line);
      PC += ARG_SIZE;
      mscan_double(assembler.virt_mem + offset, &value);
      // spu.status = check_register( &spu );//TODO add registers

      print_byn(ostream_list, spu.byte_code.desc, PUSH, value,
                assembler.virt_mem + offset);
    } else if (!my_strcmp(comm, "ADD"))
      print_byn(ostream_list, spu.byte_code.desc, ADD, 0, 0);
    else if (!my_strcmp(comm, "DIV"))
      print_byn(ostream_list, spu.byte_code.desc, DIV, 0, 0);
    else if (!my_strcmp(comm, "OUT"))
      print_byn(ostream_list, spu.byte_code.desc, OUT, 0, 0);
    else if (!my_strcmp(comm, "HLT"))
      print_byn(ostream_list, spu.byte_code.desc, HLT, 0, 0);
    else if (!my_strcmp(comm, "MULT"))
      print_byn(ostream_list, spu.byte_code.desc, MULT, 0, 0);
    else if (!my_strcmp(comm, "SQR"))
      print_byn(ostream_list, spu.byte_code.desc, SQR, 0, 0);
    else if (!my_strcmp(comm, "JMP")) {
      PC += ARG_SIZE;
      print_byn(ostream_list, spu.byte_code.desc, JMP, 0, 0);
      move_offset(&assembler, &offset, &line);
      jmp_proceed(ostream_list, &spu, &PC, &offset, &assembler, &line, &lab);
    } else if (comm[0] == ':') {
      if (add_tag(spu.byte_code.desc, comm, PC, &lab, line) == Failure)
        return {.status = Failure};
    } else {
      spu.status = Failure;
      unk_com(comm, &assembler, &line);
    }
    if (*comm != ':')
      PC += COMM_SIZE;

    move_offset(&assembler, &offset,
                &line); // Moves position to the next argument
  }

  if (spu.status != Success)
    fprintf(stderr, "Compiling failure, check log - file\n");
  printlg("---| End of compilation|---\n\n");

  SPUasm_exit(&spu);
  labels_exit(&lab);

  return spu;
}
