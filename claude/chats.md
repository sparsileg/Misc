# Managing Chats

## Executive summary

- Use the right model
- Reset chats around 15-20 messages and compact context forward
- Park shared context in Projects
- Control output length

## Session beginning and ending

- Send: Respond like a caveman. No intro. No praise. No filler. Short
  sentences. Only essential words. Skip articles when possible. Give
  an answer, not an explanation unless asked. Tell me when I've sent
  you 15 messages and then again when I've sent you 20 messages.
- Send: Respond with no intro. No praise. No filler. Short
  sentences. Only essential words.  Give an answer, not an explanation 
  unless asked. Tell me when I've sent you 15 messages and then again 
  when I've sent you 20 messages.
- **Message 10–15** → send: Give me a concise recap with decisions
  and open items.
- **Message 15–20*** → send: Summarize this entire conversation
  into a tight context block for a new chat.  Include our goal,
  current status, key decisions, what to avoid, and the very next step
  in a short, clear structure so the next AI can continue immediately
  and efficiently. Deliver it as  fenced markdown text so I can easily copy and paste it. Thank you! 
- Then paste that block into a new thread as your first message.

## Get ready for next chat

- For multi-step workflows, add a handoff prompt at natural
  boundaries: “Summarize what we’ve decided, what’s complete, and what
  the next task is so I can continue in a fresh chat.”

## Use the right model

- Haiku for brainstorming, formatting, summaries, field extraction
- Sonnet for writing, editing, analysis, coding, op workflows
- Opus for multi-step architecture, deep analysis, tasks with many
  simultaneous constraints

## Disable connectors I'm not actively using

- ~300 tokens per connector definition, loaded on every turn. Disable
  those you are not using

## Upload documents once

- If a document will be referenced more than twice, move it into the
  Project files and query it from there.

## Time of day

On weekdays, between 8am and 2pm, tokens burn faster. Be wise on how
you use them.

## Tell Claude how to respond

- Prompt: "One short paragraph, max 5 sentences."
- Prompt: "Give me the three sentence version first. If I need detail,
  I'll ask."

## Treat Claude like a strong editor

- Ask concisely and surgically
- If you have multiple questions, group them in a single message.
- Prompt: "Tighten this paragraph by ~30%"
- Prompt: "Keep structure, remove filler."

## Good prompts

- One sentence
- Only what's relevant
- Constrain by must-include / must avoid
- Think in a notepad. Decide the goal. Send the clean version.

## Use images wisely

- High-res images on Opus chew up tokens quickly. Resize if you can.
- A standard screenshot for Sonnet/Haiku can work wonders if it
  replaces a page of text.

## Custom instructions

- https://www.jdhodges.com/blog/claude-ai-custom-instructions-a-real-example-that-actually-works/

## References

- https://teammobina.substack.com/p/how-to-reduce-claude-token-usage
- https://support.claude.com/en/articles/9797557-usage-limit-best-practices
- https://www.jdhodges.com/blog/ai-session-handoffs-keep-context-across-conversations/
