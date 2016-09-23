These are various tools that I install into my `$PATH` that I find useful.

## `compress_cwd`

This compressed the value of `$PWD` so that it's not too long. I use this as an
alternative to `\w` in my prompt.

## `smartcd`

This efficiently searches `$GOPATH` to find a Go project. Recommended usage is
something like this:

```lang=bash
gocd() {
    if [ -z "${GOPATH}" ]; then
        return 1;
    fi
    local best=$(_smartcd "${GOPATH}/src" "$1")
    if [ -n "$best" ]; then
        cd "$best"
    else
        return 1
    fi
}
```

## `loaded_keys`

This prints out what SSH keys I have loaded in a form suitable to put in my
prompt.
