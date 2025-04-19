# TTC
Task tracker cli project in [roadmap.sh](https://roadmap.sh/projects/task-tracker).

## Usage

1. Compile

```bash
make
```

2. Use

```bash
# Adding a new task
task-cli.out add "Buy groceries"
# Output: Task added successfully (ID: 1)

# Updating and deleting tasks
task-cli.out update 1 "Buy groceries and cook dinner"
task-cli.out delete 1

# Marking a task as in progress or done
task-cli.out mark-in-progress 1
task-cli.out mark-done 1

# Listing all tasks
task-cli.out list

# Listing tasks by status
task-cli.out list done
task-cli.out list todo
task-cli.out list in-progress
```

## TODO

- [ ] Special encoding handle.
- [ ] Pretty output.
- [ ] Serialize formatted.

