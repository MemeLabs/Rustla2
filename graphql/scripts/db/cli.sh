#!/bin/sh

kubectl exec -it mariadb-master-0  -- mysql --user=root --password=secretpassword
