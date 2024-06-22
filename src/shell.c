#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char buf[64];
  char cmd[64];
  char arg[2][64];

  byte cwd = FS_NODE_P_ROOT;

  while (true) {
    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);

    if (strcmp(cmd, "cd")) cd(&cwd, arg[0]);
    else if (strcmp(cmd, "ls")) ls(cwd, arg[0]);
    else if (strcmp(cmd, "mv")) mv(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cp")) cp(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cat")) cat(cwd, arg[0]);
    else if (strcmp(cmd, "mkdir")) mkdir(cwd, arg[0]);
    else if (strcmp(cmd, "clear")) clearScreen();
    else printString("Invalid command\n");
  }
}

// TODO: 4. Implement printCWD function
void printCWD(byte cwd) {}

// TODO: 5. Implement parseCommand function
void parseCommand(char* buf, char* cmd, char arg[2][64]) {
  int i = 0, j = 0, k = 0;

  // Parse command
  while (buf[i] != ' ' && buf[i] != '\0') {
    cmd[j++] = buf[i++];
  }
  cmd[j] = '\0';

  // Skip spaces
  while (buf[i] == ' ') i++;

  // Parse first argument
  j = 0;
  while (buf[i] != ' ' && buf[i] != '\0') {
    arg[0][j++] = buf[i++];
  }
  arg[0][j] = '\0';

  // Skip spaces
  while (buf[i] == ' ') i++;

  // Parse second argument
  j = 0;
  while (buf[i] != ' ' && buf[i] != '\0') {
    arg[1][j++] = buf[i++];
  }
  arg[1][j] = '\0';
  printString(arg[0]);
  printString("\n");
  printString(arg[1]);
}



// TODO: 6. Implement cd function
void cd(byte* cwd, char* dirname) {
  struct node_fs node_fs_buf;
  int i;

  readSector(&node_fs_buf, FS_NODE_SECTOR_NUMBER);

  if (strcmp(dirname, "/")) {
    *cwd = FS_NODE_P_ROOT;
  }
  else if (strcmp(dirname, "..")) {
    *cwd = node_fs_buf.nodes[*cwd].parent_index;
  }
  else {
    for (i = 0; i < FS_MAX_NODE; i++) {
      if (node_fs_buf.nodes[i].parent_index == *cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname)) {
        if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR) {
          *cwd = i;
          return;
        }
        else {
          printString("cd: ");
          printString(dirname);
          printString(": Not a directory\n");
          return;
        }
      }
    }
    printString("cd: ");
    printString(dirname);
    printString(": No such file or directory\n");
  }
}




// TODO: 7. Implement ls function
void ls(byte cwd, char* dirname) {
  struct node_fs node_fs_buf;
  byte targetDir = cwd;
  int i;

  readSector(&node_fs_buf, FS_NODE_SECTOR_NUMBER);

  if (dirname[0] != '\0' && !strcmp(dirname, ".")) {
    for (i = 0; i < FS_MAX_NODE; i++) {
      if (node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname)) {
        targetDir = i;
        break;
      }
    }
    if (i == FS_MAX_NODE) {
      printString("Directory not found: ");
      printString(dirname);
      printString("\n");
      return;
    }
  }

  for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].parent_index == targetDir && node_fs_buf.nodes[i].node_name[0] != '\0') {
      printString(node_fs_buf.nodes[i].node_name);
      if (node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR) {
        printString("/");
      }
      printString("  ");
    }
  }
  printString("\n");
}




// TODO: 8. Implement mv function
void mv(byte cwd, char* src, char* dst) {
  struct node_fs node_fs_buf;
  byte srcNode, dstNode;
  int i;

  readSector(&node_fs_buf, FS_NODE_SECTOR_NUMBER);

  for(i = 0; i < FS_MAX_NODE; i++)
  {
    if(node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, src) && node_fs_buf.nodes[i].data_index != FS_NODE_D_DIR)
    {
      srcNode = i;
      break;
    }
  }

  if(i == FS_MAX_NODE)
  {
    printString("Source file not found\n");
    return;
  }

  if(strcmp(dst, "/"))
  {
    dstNode = FS_NODE_P_ROOT;
  }
  else if(strcmp(dst, ".."))
  {
    dstNode = node_fs_buf.nodes[cwd].parent_index;
  }
  else
  {
    for(i = 0; i < FS_MAX_NODE; i++)
    {
      if(node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dst) && node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR)
      {
        dstNode = i;
        break;
      }
    }

    if(i == FS_MAX_NODE)
    {
      printString("Destination directory not found\n");
      return;
    }
  }

  node_fs_buf.nodes[srcNode].parent_index = dstNode;

  writeSector(&node_fs_buf, FS_NODE_SECTOR_NUMBER);
}

// TODO: 9. Implement cp function
void cp(byte cwd, char* src, char* dst) {}

// TODO: 10. Implement cat function
void cat(byte cwd, char* filename) {}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char* dirname) {}

