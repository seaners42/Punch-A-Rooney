#include "dialogue.h"

void StartDialogue()
{
  dialogueIt = tick;
  inDialogue = true;
}

void StopDialogue()
{
  dialogueIt = tick;
  inDialogue = false;
}

void RunDialogue(const char dual_script[][max_chars], int lines, int speed, int x, int y)
{
  if (inDialogue)
  {
    int progress = (tick - dialogueIt) / speed;

    char temporaryCopy[max_chars];

    for (int i = 0; i < lines; i++)
    {
      strncpy(temporaryCopy, dual_script[std::min(progress / max_chars, lines)],
              progress < (max_chars * lines) ? progress % max_chars : max_chars);

      temporaryCopy[progress < (max_chars * lines) ? progress % max_chars : max_chars] = '\0';

      if (progress > max_chars * i)
      {
        NF_WriteText(1, 0, x, y + (i * 2),
                     progress >= (max_chars * (i + 1)) ? dual_script[i] : temporaryCopy);
      }
    }
  }
}