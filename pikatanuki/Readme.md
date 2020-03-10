

```

docker build -t pikatanuki:1.0 .
docker container run --publish 9002:9002 --detach --name pikatanuki pikatanuki:1.0
docker container rm --force pikatanuki
docker image tag pikatanuki:1.0 kaori8/pikatanuki:1.0
docker image push kaori8/pikatanuki:1.0
```

https://medium.com/serverlessguru/deploy-reactjs-app-with-s3-static-hosting-f640cb49d7e6

```
npm run build
aws s3 sync dist/ s3://tanuki.kaylee.jp --acl public-read
```
