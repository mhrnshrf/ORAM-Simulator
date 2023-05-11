from sklearn.metrics import log_loss, roc_auc_score
from sklearn.model_selection import StratifiedKFold
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import Model, load_model
from deepctr.models import DIN
from deepctr.inputs import SparseFeat,VarLenSparseFeat,DenseFeat,get_feature_names
from tensorflow.keras.callbacks import ModelCheckpoint, LearningRateScheduler, Callback
from sklearn.preprocessing import OrdinalEncoder, LabelEncoder, OneHotEncoder
from tensorflow.keras.utils import get_custom_objects
from tensorflow.keras.optimizers import Adam,RMSprop
from tensorflow.keras.layers import Activation
from tensorflow.keras import backend as K
from tensorflow.keras import callbacks
from tensorflow.keras import utils
import tensorflow.keras as keras
import tensorflow as tf
import pandas as pd
import numpy as np
import warnings
import ydata_profiling
from sklearn.metrics import log_loss, roc_auc_score
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
warnings.simplefilter('ignore')



raw_sample_df = pd.read_csv('input/raw_sample.csv')
ad_feature_df = pd.read_csv('input/ad_feature.csv')
user_profile_df=pd.read_csv('input/user_profile.csv')


test_size_mb = raw_sample_df.memory_usage().sum() / 1024 / 1024
test_size_mb1 = ad_feature_df.memory_usage().sum() / 1024 / 1024
test_size_mb2 = user_profile_df.memory_usage().sum() / 1024 / 1024
print("raw_sample_df memory size: %.2f MB" % test_size_mb)
print("ad_feature_df memory size: %.2f MB" % test_size_mb1)
print("user_profile_df memory size: %.2f MB" % test_size_mb2)


def mem_usage(pandas_obj):
    if isinstance(pandas_obj,pd.DataFrame):
        usage_b = pandas_obj.memory_usage(deep=True).sum()
    else: # we assume if not a df it's a series
        usage_b = pandas_obj.memory_usage(deep=True)
    usage_mb = usage_b / 1024 ** 2 # convert bytes to megabytes
    return "{:03.2f} MB".format(usage_mb)


raw_sample_df.info(memory_usage='deep')


optimized_gl = raw_sample_df.copy()

gl_int = raw_sample_df.select_dtypes(include=['int'])
converted_int = gl_int.apply(pd.to_numeric,downcast='unsigned')
optimized_gl[converted_int.columns] = converted_int


gl_obj = raw_sample_df.select_dtypes(include=['object']).copy()
converted_obj = pd.DataFrame()
for col in gl_obj.columns:
    num_unique_values = len(gl_obj[col].unique())
    num_total_values = len(gl_obj[col])
    if num_unique_values / num_total_values < 0.5:
        converted_obj.loc[:,col] = gl_obj[col].astype('category')
    else:
        converted_obj.loc[:,col] = gl_obj[col]
optimized_gl[converted_obj.columns] = converted_obj
print("Original Ad Feature dataframe:{0}".format(mem_usage(raw_sample_df)))
print("Memory Optimised Ad Feature dataframe:{0}".format(mem_usage(optimized_gl)))


raw_sample_df = optimized_gl.copy()
raw_sample_df_new = raw_sample_df.rename(columns = {"user": "userid"})


ad_feature_df.info(memory_usage='deep')


optimized_g2 = ad_feature_df.copy()

g2_int = ad_feature_df.select_dtypes(include=['int'])
converted_int = g2_int.apply(pd.to_numeric,downcast='unsigned')
optimized_g2[converted_int.columns] = converted_int

g2_float = ad_feature_df.select_dtypes(include=['float'])
converted_float = g2_float.apply(pd.to_numeric,downcast='float')
optimized_g2[converted_float.columns] = converted_float

print("Original Ad Feature dataframe:{0}".format(mem_usage(ad_feature_df)))
print("Memory Optimised Ad Feature dataframe:{0}".format(mem_usage(optimized_g2)))


user_profile_df.info(memory_usage='deep')


optimized_g3 = user_profile_df.copy()

g3_int = user_profile_df.select_dtypes(include=['int'])
converted_int = g3_int.apply(pd.to_numeric,downcast='unsigned')
optimized_g3[converted_int.columns] = converted_int

g3_float = user_profile_df.select_dtypes(include=['float'])
converted_float = g3_float.apply(pd.to_numeric,downcast='float')
optimized_g3[converted_float.columns] = converted_float

print("Original User Feature dataframe:{0}".format(mem_usage(user_profile_df)))
print("Memory Optimised User Feature dataframe:{0}".format(mem_usage(optimized_g3)))

df1 = raw_sample_df_new.merge(optimized_g3, on="userid")
final_df = df1.merge(optimized_g2, on="adgroup_id")
final_df.head()

final_df['hist_cate_id'] = final_df['cate_id']
final_df['hist_adgroup_id'] = final_df['adgroup_id']

sparse_features = [feat for feat in final_df.columns if feat not in ['time_stamp','pid', 'nonclk','brand',
       'cms_segid', 'cms_group_id', 'age_level',
       'pvalue_level', 'shopping_level', 'occupation', 'new_user_class_level ',
        'campaign_id', 'customer', 'price', 'hist_cate_id','hist_adgroup_id']]
sparse_features

dense_features = [feat for feat in final_df.columns if feat not in ['userid', 'time_stamp', 'adgroup_id', 'pid', 'nonclk', 'clk',
       'cms_segid', 'cms_group_id', 'final_gender_code', 'age_level',
       'pvalue_level', 'shopping_level', 'occupation', 'new_user_class_level ',
       'cate_id', 'campaign_id', 'customer', 'brand','hist_cate_id','hist_adgroup_id']]
dense_features

sequence_features = [feat for feat in final_df.columns if feat not in ['userid', 'time_stamp', 'adgroup_id', 'pid', 'nonclk', 'clk',
       'cms_segid', 'cms_group_id', 'final_gender_code', 'age_level',
       'pvalue_level', 'shopping_level', 'occupation', 'new_user_class_level ',
       'cate_id', 'campaign_id', 'customer', 'brand', 'price']]
sequence_features

behavior_feature_list = [feat for feat in final_df.columns if feat in ['adgroup_id', 'cate_id']]
behavior_feature_list

final_df[sparse_features] = final_df[sparse_features].fillna('-1', )
final_df[sequence_features] = final_df[sequence_features].fillna('-1', )
final_df[dense_features] = final_df[dense_features].fillna(0, )
target = ['clk']


mms = MinMaxScaler(feature_range=(0, 1))
final_df[dense_features] = mms.fit_transform(final_df[dense_features])


fixlen_feature_columns = [SparseFeat(feat, vocabulary_size=2000000,embedding_dim=8) for feat in sparse_features] + [DenseFeat(feat, 1, )for feat in dense_features] + [VarLenSparseFeat(SparseFeat(feat, vocabulary_size=2000000,embedding_dim=8), maxlen=1) for feat in sequence_features] 
linear_feature_columns = fixlen_feature_columns
dnn_feature_columns = fixlen_feature_columns
feature_names = get_feature_names(linear_feature_columns + dnn_feature_columns, )

train, test = train_test_split(final_df, test_size=0.5)
train_model_input = {name:train[name] for name in feature_names}
test_model_input = {name:test[name] for name in feature_names}

print("######################## MODEL ###############################")

model = DIN(linear_feature_columns, behavior_feature_list, dnn_use_bn=True,
        dnn_hidden_units=(200, 80), dnn_activation='relu', att_hidden_size=(80, 40), att_activation="dice",
        att_weight_normalization=False, l2_reg_dnn=0, l2_reg_embedding=1e-6, dnn_dropout=0, init_std=0.0001, seed=1024,
        task='binary')


model.summary()


for name, value in test_model_input.items():
    print(name + ':')
    print(value)
    print(value.shape)
    print('Range:', min(value), '~', max(value))
    print('Sub Name Range:', min(value.keys()), '~', max(value.keys()))


# # Open a new file for writing
# with open("userid.txt", "w") as file:
#     # Loop through the data and write each line to the file
#     for name, value in test_model_input.items():
#         if name == "userid":
#             if isinstance(value, dict):
#                 for sub_name, sub_value in value.items():
#                     file.write(str(sub_value) + '\n')
#     file.close()

# with open("adgroup_id.txt", "w") as file:
#     # Loop through the data and write each line to the file
#     for name, value in test_model_input.items():
#         if name == "adgroup_id":
#             if isinstance(value, dict):
#                 for sub_name, sub_value in value.items():
#                     file.write(str(sub_value) + '\n')
#     file.close()


# with open("hist_adgroup_id.txt", "w") as file:
#     # Loop through the data and write each line to the file
#     for name, value in test_model_input.items():
#         if name == "hist_adgroup_id":
#             if isinstance(value, dict):
#                 for sub_name, sub_value in value.items():
#                     file.write(str(sub_value) + '\n')
#     file.close()


# Loop through the data and write each line to the file
import random

for name, value in test_model_input.items():
    with open(name + ".txt", "w") as file:
        lines = value.to_string(index=False).strip().split('\n')
        for line in lines:
            num = int(float(line.strip()))
            num = num<<6
            hex_string = hex(num)[2:]
            nonmemops = random.randint(1, 300)
            file.write(str(nonmemops) + " R 0x" + hex_string + " 0x1" + "\n")
        file.close()


for name, value in test_model_input.items():
    with open(name + "-raw.txt", "w") as file:
        lines = value.to_string(index=False).strip().split('\n')
        for line in lines:
            num = int(float(line.strip()))
            file.write(str(num) + "\n")
        file.close()




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
