# Classifiers

Classifiers a simple text-files that are used to further parse the contents
of messages. Two types exist:
- System message classifiers (`system_*.txt`)
- Nickname change parsers (`nickname_*.txt`)

### System message classifiers
System message classifiers are contained in files named according to the following
pattern: `system_[lang].txt`. They are used to flag matching messages as system
messages, changing their rendering and excluding them from word counts, for example.

This type of file should contain one regular expression per line. If the expression
matches the message content, the message is flagged as a system message.

Example:
```regexp
.*? created the group\.?
.*? changed the group photo\.?
.*? named the group .*\.?
```

This would mean that messages such as `John Smith created the group.`, `Someone changed the group photo.`, and `You named the group Something.` would be marked 
as system messages and not as messages sent by a person.

### Nickname change parsers
Nickname change parsers are somewhat more complicated, and are used to detect
nickname changes in messages (obviously). They are contained in files named 
according to this format: `nickname_[lang].txt`.

They contain regular expressions like the system classifiers, but they capture
information, and furthermore contain more patterns to match special cases of
nickname changes (like someone changing your nickname, or someone changing their
own nickname).

Example (the file for English):

```
.*? set the nickname for (?'subject'.*?) to (?'nickname'.*)\.
.*? set (?'subject'his|her|their) own nickname to (?'nickname'.*)\.
.*? set (?'subject'your) nickname to (?'nickname'.*)\.

# (?:his|her|their) -> sender
# your -> owner
```

Lines not starting with `#` are the patterns which determine if the message is a
nickname change, and capture information. The person whose nickname has been changed
should be in a named group called `subject`, and the actual nickname should be in a 
named group called `nickname`.

Lines starting with a `#` are used to further process the subject of the change. 
They follow this format: `# (pattern) -> (special type)`, where special type can be `owner`, for the owner of the data download (you), and `sender`, which marks the 
sender of the message (the person making the change).

If the subject captured in the message matches any of these patterns, then the
special type designated by that pattern will be used instead of the name captured
by the matching expressions.

For example, let's see these messages:
- `John set the nickname for Adam to ABC.`. According to the first pattern in the file, `subject` will be `Adam`, and `nickname` will be `ABC`. Since
we have a concrete name as the subject, we stop here.
- `John set his own nickname to ABC.`. In this message, subject will be `his`, and the nickname will be `ABC` (note that we can't capture the first word
in the message as the subject, since that can contain a previously set nickname). We need to do some more processing here, since our subject is `his`, not
a concrete person. This is where the first line starting with `#` comes in, as it matches our current subject, and so the subject will be interpreted by
the program as the sender of this message.
- `John set your nickname to ABC.`. This case is the same as the above, with subject being `your`. The second line starting with `#` will match this, and
override the subject as the owner of the data download (you).
