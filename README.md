# Stanford CS144

**Since 2023-12-16 cs144/minnow made private, I use cherry pick or merge to merge startercode from others.**

> Finally I found https://github.com/fei0319/CS144 which stores all the checkpoint branches.

Show all commits in the merge commit:

```txt
git log df53862^..df53862
```

Cherry pick to apply the merge commits in order (not include merge commit):

```txt
git cherry-pick start..end
git cherry-pick start~..end  # include start
```
