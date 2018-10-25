import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import idx from 'idx';

import { fetchPoll, submitPollVote } from '../actions';
import MainLayout from './MainLayout';
import Checkbox from './Checkbox';


const SingleSelectInput = ({ value }) => (
  <div className='form-group'>
    <label>
      <input
        className='form-control form-radio-input poll-vote-input'
        name='options'
        value={value}
        type='radio'
      />
      <span className='poll-vote-label'>{value}</span>
    </label>
  </div>
);

SingleSelectInput.propTypes = {
  value: PropTypes.string.isRequired,
};

const MultiSelectInput = ({ value }) => (
  <div className='form-group col-sm-12'>
    <label className='poll-vote-row'>
      <Checkbox
        className='form-control poll-vote-input'
        name={`options`}
        value={value}
      />
      <span className='poll-vote-label'>{value}</span>
    </label>
  </div>
);

MultiSelectInput.propTypes = {
  value: PropTypes.string.isRequired,
};

class PollForm extends React.Component {
  handleSubmit = (event) => {
    event.preventDefault();

    const options = Array.from(event.target.elements)
      .filter(el => el.name === 'options' && el.checked)
      .map(el => el.value);

    if (options.length === 0) {
      return;
    }

    this.props.onSubmit({options});
  }

  render() {
    const {
      subject,
      options,
      multi_vote: multiVote,
    } = this.props.poll;

    const Input = multiVote ? MultiSelectInput : SingleSelectInput;
    const optionInputs = Object.entries(options).map(([value], i) => (
      <Input key={i} value={value} />
    ));

    return (
      <form onSubmit={this.handleSubmit}>
        <h1 className='col-sm-12'>{subject}</h1>
        {optionInputs}
        <div className='form-group'>
          <div className='col-sm-12'>
            <button
              type='submit'
              className='btn btn-primary'
              disabled={false}
            >
              Submit
            </button>
          </div>
        </div>
      </form>
    );
  }
}

PollForm.propTypes = {
  onSubmit: PropTypes.func.isRequired,
  poll: PropTypes.object.isRequired,
};

const PollVote = ({ poll, history, submitPollVote }) => {
  const pollForm = poll && !poll.loading
    ? <PollForm onSubmit={submitPollVote} poll={poll} />
    : <h4>Loading...</h4>;

  return (
    <MainLayout history={history}>
      <div className='container'>
        {pollForm}
      </div>
    </MainLayout>
  );
};

PollVote.propTypes = {
  poll: PropTypes.object,
  history: PropTypes.object.isRequired,
};

function mapDispatchToProps(dispatch, props) {
  return {
    submitPollVote(options) {
      return dispatch(submitPollVote(
        props.match.params.poll,
        options,
        props.history,
      ));
    },
    fetchPoll(id) {
      return dispatch(fetchPoll(id));
    },
  };
}

export default compose(
  connect(
    (state, ownProps) => {
      const id = ownProps.match.params.poll;
      return {
        id,
        poll: idx(state, _ => _.polls[id]),
      };
    },
    mapDispatchToProps,
  ),
  lifecycle({
    componentDidMount() {
      this.props.fetchPoll(this.props.id);
    },
  }),
)(PollVote);
