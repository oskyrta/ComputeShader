import json

values_dict = {}

with open('copy_from_gpu_log.json', 'r', encoding='utf-8') as f:
    file_data = '[' + f.read()[0:-2] + ']'
    values = json.loads(file_data)

    for value in values:
        if value['type'] == 'gpu':
            i = int(value['objects_count'])
            if i not in values_dict.keys() :
                values_dict[i] = []

            values_dict[i].append(float(value['avg_time']))

    keys = [x for x in values_dict.keys()]
    keys.sort()
    for i in keys:
        print('{}, {}'.format( i, sum(values_dict[i]) / len(values_dict[i]) ))