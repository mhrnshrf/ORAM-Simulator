import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from xgboost import XGBClassifier
import xgboost as xgb
from deepctr.models.wdl import WDL
from deepctr.feature_column import SparseFeat, DenseFeat, get_feature_names
from sklearn.metrics import roc_curve,log_loss
from sklearn.preprocessing import OneHotEncoder,LabelEncoder, MinMaxScaler
from sklearn.model_selection import GridSearchCV
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
import time
from sklearn.model_selection import KFold
continuous_variable = [f"I{i}" for i in range(1, 14)]
discrete_variable = [f"C{i}" for i in range(1, 27)]
train_df_cols = ["Label"] + continuous_variable + discrete_variable
test_df_cols = continuous_variable + discrete_variable
train_df = pd.read_csv("dac/train.txt", sep='\t', names=train_df_cols, nrows=1000000)
train_df.head()
test_df = pd.read_csv("dac/test.txt", sep='\t', names=test_df_cols)
print(f"train_df has {train_df.shape[0]} rows and {train_df.shape[1]} columns.")
print(f"test_df has {test_df.shape[0]} rows and {test_df.shape[1]} columns.")
for col in train_df.columns:
    if train_df[col].dtypes == "object":
        train_df[col], uniques = pd.factorize(train_df[col])
    train_df[col].fillna(train_df[col].mean(), inplace=True)
train_df.head()
train_df.info()
count_label = pd.value_counts(train_df["Label"], sort=True)
count_label.plot(kind="bar")    # 条形图
plt.title("Label Statistics")
plt.xlabel("Label")
plt.ylabel("Frequency")
#负样本:正样本 = 2.92:1（scale_pos_weight=2.92）
pd.value_counts(train_df["Label"], sort=True)
# 下采样
number_of_click = len(train_df[train_df.Label == 1])    # 统计点击的数据量
click_indices = np.array(train_df[train_df.Label == 1].index)    # 点击的数据索引
no_click_indices = np.array(train_df[train_df.Label == 0].index)   # 正常数据索引

random_no_click_indices = np.array(np.random.choice(no_click_indices, number_of_click, replace=False))

under_sample_indices = np.concatenate([click_indices, random_no_click_indices])
under_sample_train_df = train_df.iloc[under_sample_indices, :]

X_under_sample = under_sample_train_df.iloc[:, under_sample_train_df.columns != "Label"]
Y_under_sample = under_sample_train_df.iloc[:, under_sample_train_df.columns == "Label"]
print("Total number of under_sample_train_df =", len(under_sample_train_df))
print("Total number of no_click =", len(under_sample_train_df[train_df.Label == 0]))
print("Total number of click =", len(under_sample_train_df[train_df.Label == 1]))
#train_x, train_y = train_df.iloc[:, train_df.columns != "Label"], train_df.iloc[:, train_df.columns == "Label"]
#train_y = np.array(train_y).ravel()
#Y_under_sample = np.array(Y_under_sample).ravel()
X_under_sample.head()
Y_under_sample
X_under_sample_train,X_under_sample_test,y_under_sample_train,y_under_sample_test = train_test_split(X_under_sample,Y_under_sample,test_size=0.3)
print(f" X_under_sample_train:{X_under_sample_train.shape}\n X_under_sample_test:{X_under_sample_test.shape}\n y_under_sample_train:{y_under_sample_train.shape}\n y_under_sample_test:{y_under_sample_test.shape}")
y_under_sample_train.shape
X_under_sample_train

data = pd.concat([X_under_sample,Y_under_sample],axis=1)
data.head()
sparse_features = ['C' + str(i) for i in range(1, 27)]
dense_features = ['I' + str(i) for i in range(1, 14)]

data[sparse_features] = data[sparse_features].fillna('-1', )
data[dense_features] = data[dense_features].fillna(0, )
target = ['Label']

for feat in sparse_features:
    lbe = LabelEncoder()
    data[feat] = lbe.fit_transform(data[feat])

mms = MinMaxScaler(feature_range=(0, 1))
data[dense_features] = mms.fit_transform(data[dense_features])

sparse_feature_columns = [SparseFeat(feat, vocabulary_size=data[feat].nunique(), embedding_dim=4)
                          for i, feat in enumerate(sparse_features)]
# sparse_feature_columns = [SparseFeat(feat, vocabulary_size=1e6,embedding_dim=4,use_hash=True)
#                            for i,feat in enumerate(sparse_features)]#The dimension can be set according to data
dense_feature_columns = [DenseFeat(feat, 1)
                         for feat in dense_features]

dnn_feature_columns = sparse_feature_columns + dense_feature_columns
linear_feature_columns = sparse_feature_columns + dense_feature_columns
feature_names = get_feature_names(linear_feature_columns + dnn_feature_columns)

train, test = train_test_split(data, test_size=0.99)

train_model_input = {name: train[name] for name in feature_names}
test_model_input = {name: test[name] for name in feature_names}

for name, value in test_model_input.items():
    print(f"Feature: {name}")
    print("Values:", value)
    print("=" * 50)  # Separator for better readability
    print(value.shape)
    print('Range:', min(value), '~', max(value))
    print('Sub Name Range:', min(value.keys()), '~', max(value.keys()))

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


# model = WDL(linear_feature_columns, dnn_feature_columns, task='binary')
# model.compile("adam", "binary_crossentropy",
#               metrics=['binary_crossentropy'], )

# history = model.fit(train_model_input, train[target].values,
#                     batch_size=256, epochs=10, verbose=2, validation_split=0.2, )
# pred_ans = model.predict(test_model_input, batch_size=256)
# pred_ans = model.predict(test_model_input, batch_size=256)
# print("test LogLoss", round(log_loss(test[target].values, pred_ans), 4))
# print("test AUC", round(roc_auc_score(test[target].values, pred_ans), 4))
# pred_ans2 = []
# for i in pred_ans:
#     if i>=0.5:
#         pred_ans2.append(1)
#     else:
#         pred_ans2.append(0)
# #print(pred_ans2)
# eval_model(pred_ans2,test[target].values)

