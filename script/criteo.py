# https://blog.csdn.net/zhong_ddbb/article/details/107964654
# 数据探索
import pandas as pd
import numpy as np

sparse_feats = ['C' + str(i) for i in range(1, 27)]
dense_feats = ['I' + str(i) for i in range(1, 14)]
target_columns = ['label']
columns = target_columns + dense_feats + sparse_feats

train = pd.read_csv("/kaggle/input/criteo-dataset/dac/train.txt",sep='\t',names = columns,nrows=20000)
pd.set_option('display.max_columns', None)  # 显示完整的列
pd.set_option('display.max_rows', None)  # 显示完整的行
pd.set_option('display.expand_frame_repr', False)  # 设置不折叠数据
pd.set_option('display.max_colwidth', 100) # 设置行宽度
train.head(2)



from sklearn.preprocessing import LabelEncoder
def process_dense_feats(data,feats):
    d  = data.copy()
    d = d[feats].fillna(0.0)
    for f in feats:
        d[f] = d[f].apply(lambda x: np.log(x+1) if x>-1 else -1)
    return d
data_dense = process_dense_feats(train, dense_feats)

def process_spares_feats(data,feats):
    d = data.copy()
    d = d[feats].fillna('-1')
    for f in feats:
        d[f] = LabelEncoder().fit_transform(d[f]) #https://zhuanlan.zhihu.com/p/33569866  LabelEncoder VS OneHotEncoder
    return d
data_sparse = process_spares_feats(train,sparse_feats)
total_data = pd.concat([data_dense,data_sparse],axis=1)
total_data['label'] = train['label']
total_data.head(2)



import tensorflow as tf
from tensorflow.keras.layers import *

sparse_inputs = []
for f in sparse_feats:
    _input = Input([1], name=f)
    sparse_inputs.append(_input)
    
sparse_1d_embed = []
for i, _input in enumerate(sparse_inputs):
    f = sparse_feats[i]
    voc_size = total_data[f].nunique()
    reg = tf.keras.regularizers.l2(0.5)
    _embed = Embedding(voc_size, 1, embeddings_regularizer=reg)(_input)
    _embed = Flatten()(_embed)
    sparse_1d_embed.append(_embed)
# print(sparse_1d_embed)
fst_order_sparse_layer = Add()(sparse_1d_embed)

dense_inputs = []
for f in dense_feats:
    _input = Input([1], name=f)
    dense_inputs.append(_input)
concat_dense_inputs = Concatenate(axis=1)(dense_inputs)  # ?, 13
fst_order_dense_layer = Dense(1)(concat_dense_inputs)  # ?, 1


linear_part = Add()([fst_order_dense_layer, fst_order_sparse_layer])

# embedding size
k = 8

sparse_kd_embed = []
for i, _input in enumerate(sparse_inputs):
    f = sparse_feats[i]
    voc_size = total_data[f].nunique()
    reg = tf.keras.regularizers.l2(0.7)
    _embed = Embedding(voc_size, k, embeddings_regularizer=reg)(_input)
    sparse_kd_embed.append(_embed)


import tensorflow.keras.backend as K 
from tensorflow.keras.models import Model
# 1.将所有 sparse 特征 (?, 1, k)的embedding拼接起来，
# 得到 (?, n, k)的矩阵，其中n为特征数，k为embedding大小
concat_sparse_kd_embed = Concatenate(axis=1)(sparse_kd_embed)  # ?, n, k

# 2.先求和再平方
sum_kd_embed = Lambda(lambda x: K.sum(x, axis=1))(concat_sparse_kd_embed)  # ?, k
square_sum_kd_embed = Multiply()([sum_kd_embed, sum_kd_embed])  # ?, k

# 3.先平方再求和
square_kd_embed = Multiply()([concat_sparse_kd_embed, concat_sparse_kd_embed]) # ?, n, k
sum_square_kd_embed = Lambda(lambda x: K.sum(x, axis=1))(square_kd_embed)  # ?, k

# 4.相减除以2
sub = Subtract()([square_sum_kd_embed, sum_square_kd_embed])  # ?, k
sub = Lambda(lambda x: x*0.5)(sub)  # ?, k
snd_order_sparse_layer = Lambda(lambda x: K.sum(x, axis=1, keepdims=True))(sub)  # ?, 1

flatten_sparse_embed = Flatten()(concat_sparse_kd_embed)  # ?, n*k
fc_layer = Dropout(0.5)(Dense(256, activation='relu')(flatten_sparse_embed))  # ?, 256
fc_layer = Dropout(0.3)(Dense(128, activation='relu')(fc_layer))  # ?, 128
fc_layer = Dropout(0.1)(Dense(64, activation='relu')(fc_layer))  # ?, 64
fc_layer_output = Dense(1)(fc_layer)  # ?, 1

output_layer = Add()([linear_part, snd_order_sparse_layer, fc_layer_output])
output_layer = Activation("sigmoid")(output_layer)

model = Model(dense_inputs+sparse_inputs, output_layer)
model.compile(optimizer="adam", 
              loss="binary_crossentropy", 
              metrics=["binary_crossentropy", tf.keras.metrics.AUC(name='auc')])



from matplotlib import pyplot as plt
tf.keras.utils.plot_model(model, to_file='model.png', show_shapes=True)

from sklearn.model_selection import train_test_split

x_data, x_test, y_data, y_test = train_test_split(total_data[dense_feats+sparse_feats],total_data[target_columns],test_size = 0.3,random_state=0)
x_data.shape,y_data.shape

train_dense_x = [x_data[f].values for f in dense_feats]
train_sparse_x = [x_data[f].values for f in sparse_feats]
train_label = y_data

val_dense_x = [x_test[f].values for f in dense_feats]
val_sparse_x = [x_test[f].values for f in sparse_feats]
val_label = y_test

# model.fit(train_dense_x+train_sparse_x, 
#           train_label, epochs=5, batch_size=256,
#           validation_data=(val_dense_x+val_sparse_x, val_label),
#          )


# model.predict(val_dense_x+val_sparse_x)



# Loop through the data and write each line to the file
# import random

# for name, value in test_model_input.items():
#     with open(name + ".txt", "w") as file:
#         lines = value.to_string(index=False).strip().split('\n')
#         for line in lines:
#             num = int(float(line.strip()))
#             num = num<<6
#             hex_string = hex(num)[2:]
#             nonmemops = random.randint(1, 300)
#             file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")
#         file.close()


# for name, value in test_model_input.items():
#     with open(name + "-raw.txt", "w") as file:
#         lines = value.to_string(index=False).strip().split('\n')
#         for line in lines:
#             num = int(float(line.strip()))
#             file.write(str(num) + "\n")
#         file.close()




print("######################## TRAIN ###############################")

# model.compile("adam", "binary_crossentropy",metrics=['binary_crossentropy'], )
# history = model.fit(train_model_input, train[target].values,batch_size=5024, epochs=5, verbose=1, validation_split=0.25, )

print("######################## Test ###############################")

# pred_ans = model.predict(test_model_input, batch_size=256)

# print("test LogLoss", round(log_loss(test[target].values, pred_ans), 2))
# print("test AUC", round(roc_auc_score(test[target].values, pred_ans), 2))


# for layer in model.layers:
#     if hasattr(layer, 'embeddings'):
#         intermediate_layer_model = tf.keras.Model(inputs=model.input,
#                                             outputs=layer.output)
#         intermediate_output = intermediate_layer_model.predict(test_model_input)
#         print("Layer Name:", layer.name)
#         print("Input Shape:", intermediate_output.shape)
#         print("Input:", intermediate_output)
