import ArtusConfigBase as base
import mc11


def config():
    conf = mc11.config()
    l = []
    for pipeline in conf['Pipelines']:
        if not pipeline.startswith('all'):
            l.append(pipeline)
        #elif pipeline.endswith('L3'):
        #    l.append(pipeline)
        elif 'CHS' not in pipeline:
            l.append(pipeline)
    for pipeline in l:
            del conf['Pipelines'][pipeline]
    for pipeline in conf['Pipelines']:
        conf['Pipelines'][pipeline]['Consumer'] = [
            "muonntuple",
            #"jetntuple",
        ]
    return conf

