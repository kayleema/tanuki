```
docker build -t kaori8/tanukisekai .
source runtime.env
docker run -p 49160:3000 -d --env-file ./runtime.env kaori8/tanukisekai
```
