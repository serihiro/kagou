# Repository Guidelines for Codex Agents

This repository contains a tiny C web server and related tests.

## Key directories

- `src` - C source code for the server
- `tests` - unit tests written in C
- `e2e` - end-to-end tests using Playwright
- `test` - static test assets served by the server

## Development workflow

1. Build the project with `make`.
2. Format C source files before committing using `make format`.
3. Run unit tests with `make test`.
4. Run end-to-end tests with `yarn e2e`. If dependencies are missing, run `yarn install` and `yarn playwright install` first.
5. Ensure all tests pass before opening a pull request.

## Pull request notes

- Summarize major changes in the PR description.
- Include the results of `make test` and `yarn e2e` in the testing section of the PR body.

