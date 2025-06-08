# Kagou

- [Kagou](https://ja.wikipedia.org/wiki/%E3%82%AB%E5%8F%B7%E8%A6%B3%E6%B8%AC%E6%A9%9F) is a tiny web server written in c.

## Usage

```
$ git clone git@github.com:serihiro/kagou.git
$ cd Kagou
$ make
$ ./kagou 5000 html
```

### E2E tests

Install JavaScript dependencies and browsers using **yarn**:

```
$ yarn install
$ yarn playwright install
```

Run the Playwright tests:

```
$ yarn e2e
```

## Features(including plan)

### Support media
- [x] html
- [x] js
- [x] css
- [x] csv
- [x] json
- [x] jpg
- [x] png
- [x] gif

### Support options
- [x] specify port
- [x] specify base directory path

### Support http status
- [x] 200
- [ ] 400
- [ ] 401
- [ ] 403
- [x] 404
- [ ] 405
- [x] 415

- [x] 500

### Other features
- [ ] logging
- [ ] demonize
- [ ] support keep-alive
- [ ] support cache-control
- [ ] support config file
- [ ] support basic auth

## License

MIT
