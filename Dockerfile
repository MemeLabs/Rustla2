FROM node:12.22.8-alpine AS build

RUN mkdir /ui
WORKDIR /ui

COPY public ./public
COPY src ./src
COPY \
  .babelrc \
  .eslintrc.js \
  .flowconfig \
  package-lock.json \
  package.json \
  webpack.config.js \
  ./

ENV ENV_SRC=".env.prod"
COPY ${ENV_SRC} .env

RUN npm install
RUN npm run build:production

FROM nginx:stable-alpine

COPY --from=build /ui/public /usr/share/nginx/html/
