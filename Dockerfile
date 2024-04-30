FROM alpine:latest

WORKDIR /app

COPY . /app

RUN apk --no-cache add git
