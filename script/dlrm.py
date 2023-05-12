import tensorflow as tf

!wget -c https://labs.criteo.com/wp-content/uploads/2015/04/dac_sample.tar.gz -O - | tar -xz
# !wget -c http://go.criteo.net/criteo-research-kaggle-display-advertising-challenge-dataset.tar.gz  -O - | tar -xz

import pandas as pd
from sklearn import preprocessing

# Load the dataset and fill the nan with 0
columns = ['label', *(f'I{i}' for i in range(1, 14)), *(f'C{i}' for i in range(1, 27))]
df = pd.read_csv('dac_sample.txt', sep='\t', names=columns).fillna(0)
df

# Preprocess Dense Features
dense_cols = [c for c in columns if 'I' in c]
df[dense_cols] = preprocessing.StandardScaler().fit_transform(df[dense_cols])
df


# Preprocess Categorical Features
cat_cols = [c for c in columns if 'C' in c]
mappings = {
            col: dict(zip(values, range(len(values))))
                for col, values in map(lambda col: (col, df[col].unique()), cat_cols)
                }
for col, mapping in mappings.items():
        df[col] = df[col].map(mapping.get)
        df

label_counts = df.groupby('label')['I1'].count()
print(f'Baseline: {max(label_counts.values) / sum(label_counts.values) * 100}%')

dense_cols = [c for c in df.columns if 'I' in c]
cat_cols = [c for c in df.columns if 'C' in c]
emb_counts = [len(df[c].unique()) for c in cat_cols]

from keras.utils.np_utils import to_categorical

TRAIN_SPLIT = 0.2
VALIDATION_SPLIT = 0.5

ds = tf.data.Dataset.zip((
        tf.data.Dataset.from_tensor_slices((
                    tf.cast(df[dense_cols].values, tf.float32),
                            tf.cast(df[cat_cols].values, tf.int32),
                                )),
            tf.data.Dataset.from_tensor_slices((
                        tf.cast(to_categorical(df['label'].values, num_classes=2), tf.float32)
                            ))
            )).shuffle(buffer_size=2048)


ds_test = ds.take(int(len(ds) * TRAIN_SPLIT))
ds_train = ds.skip(len(ds_test))
ds_valid = ds_test.take(int(len(ds_test) * VALIDATION_SPLIT))
ds_test = ds_test.skip(len(ds_valid))


def MLP(arch, activation='relu', out_activation=None):
    mlp = tf.keras.Sequential()

    for units in arch[:-1]:
        mlp.add(tf.keras.layers.Dense(units, activation=activation))

    mlp.add(tf.keras.layers.Dense(arch[-1], activation=out_activation))

    return mlp


class SecondOrderFeatureInteraction(tf.keras.layers.Layer):
    def __init__(self, self_interaction=False):
        super(SecondOrderFeatureInteraction, self).__init__()
        self.self_interaction = self_interaction

    def call(self, inputs):
        batch_size = tf.shape(inputs[0])[0]
        concat_features = tf.stack(inputs, axis=1)

        dot_products = tf.matmul(concat_features, concat_features, transpose_b=True)

        ones = tf.ones_like(dot_products)
        mask = tf.linalg.band_part(ones, 0, -1)
        out_dim = int(len(inputs) * (len(inputs) + 1) / 2)

        if not self.self_interaction:
            mask = mask - tf.linalg.band_part(ones, 0, 0)
            out_dim = int(len(inputs) * (len(inputs) - 1) / 2)

        flat_interactions = tf.reshape(tf.boolean_mask(dot_products, mask), (batch_size, out_dim))
        return flat_interactions


class DLRM(tf.keras.Model):
    def __init__(
            self,
            embedding_sizes,
            embedding_dim,
            arch_bot,
            arch_top,
            self_interaction,
    ):
        super(DLRM, self).__init__()
        self.emb = [tf.keras.layers.Embedding(size, embedding_dim) for size in embedding_sizes]
        self.bot_nn = MLP(arch_bot, out_activation='relu')
        self.top_nn = MLP(arch_top, out_activation='sigmoid')
        self.interaction_op = SecondOrderFeatureInteraction(self_interaction)

    def call(self, input):
        input_dense, input_cat = input
        emb_x = [E(x) for E, x in zip(self.emb, tf.unstack(input_cat, axis=1))]
        dense_x = self.bot_nn(input_dense)

        Z = self.interaction_op(emb_x + [dense_x])
        z = tf.concat([dense_x, Z], axis=1)
        p = self.top_nn(z)

        return p
		
		
model = DLRM(
    embedding_sizes=emb_counts,
    embedding_dim=2,
    arch_bot=[8, 2],
    arch_top=[128, 64, 2],
    self_interaction=False
)

model.compile(
    optimizer=tf.keras.optimizers.Adam(learning_rate=0.0001),
    loss=tf.keras.losses.BinaryCrossentropy(from_logits=True),
    metrics=['accuracy']
)


BATCH_SIZE = 128

history =model.fit(
    ds_train.batch(BATCH_SIZE),
    validation_data=ds_valid.batch(BATCH_SIZE),
    callbacks=[
        tf.keras.callbacks.EarlyStopping(patience=6, restore_best_weights=True)
    ],
    epochs=100,
    verbose=1,
)

model.summary()


results = model.evaluate(ds_test.batch(BATCH_SIZE))
print(f'Loss {results[0]}, Accuracy {results[1]}')



