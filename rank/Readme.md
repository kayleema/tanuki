```
docker build -t kaori8/tanukisekai:1.0 .

source runtime.env
docker run -p 49160:3000 -d --env-file ./runtime.env  --name tanukisekai kaori8/tanukisekai:1.0
docker container rm --force tanukisekai

docker image push kaori8/tanukisekai:1.0
aws ecs update-service --cluster maincluster --service tanuki_sekai_service --force-new-deployment
```
